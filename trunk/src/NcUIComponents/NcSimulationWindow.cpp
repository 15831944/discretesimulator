#include "NcUIComponents\NcSimulationWindow.h"
#include "NcSimulationController\NcSimulationController.h"
#include "NcDisplay\NcDisplay.h"
#include "NcStateMachine\NcCode.h"
#include <math.h>
#include <QMouseEvent>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <QPainter>
#include <QDebug>


using namespace DiscreteSimulator;

NcSimulationWindow::NcSimulationWindow(QWidget *parent) 
: QGLWidget(parent)
{
	//setFormat(QGLFormat(QGL::SingleBuffer | QGL::DepthBuffer));
	mStartSimulation = false;
	gstepmode = false;
	setFocusPolicy(Qt::ClickFocus);
	mSimStepMode = false;
	mIsNewFile = false;
	
}

void	NcSimulationWindow::setIsNewFile( bool isnewfile)
{
	mIsNewFile = isnewfile;
}


//NcSimulationWindow::~NcSimulationWindow()
//{
//	/*glDeleteLists(backgroundDL, 1);*/
//}



void	NcSimulationWindow::initLighting( )
{
	makeCurrent();
	GLfloat global_ambient[4]={0.2f, 0.2f,  0.2f, 1.0f};		// Set Ambient Lighting To Fairly Dark Light (No Color)
	GLfloat light0ambient[4]= {0.2f, 0.2f,  0.2f, 1.0f};		// More Ambient Light
	GLfloat light0diffuse[4]= {0.3f, 0.3f,  0.3f, 1.0f};		// Set The Diffuse Light A Bit Brighter
	GLfloat light0specular[4]={0.8f, 0.8f,  0.8f, 1.0f};		// Fairly Bright Specular Lighting

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);		// Set The Global Ambient Light Model
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0ambient);			// Set The Ambient Light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0diffuse);			// Set The Diffuse Light
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0specular);			// Set Up Specular Lighting
	glEnable(GL_LIGHT0);										// Enable Light0
	
	glLightfv(GL_LIGHT1, GL_AMBIENT, light0ambient);			// Set The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light0diffuse);			// Set The Diffuse Light
	glLightfv(GL_LIGHT1, GL_SPECULAR, light0specular);			// Set Up Specular Lighting
	glEnable(GL_LIGHT1);										// Enable Light0

	glLightfv(GL_LIGHT2, GL_AMBIENT, light0ambient);			// Set The Ambient Light
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light0diffuse);			// Set The Diffuse Light
	glLightfv(GL_LIGHT2, GL_SPECULAR, light0specular);			// Set Up Specular Lighting
	glEnable(GL_LIGHT2);										// Enable Light0
}
void	NcSimulationWindow::initializeGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);								// Black Background
	glClearDepth (1.0f);												// Depth Buffer Setup
	glEnable (GL_DEPTH_TEST);											// Enable Depth Testing
	glDepthFunc (GL_LEQUAL);											// The Type Of Depth Testing
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);					// Really Nice Perspective Calculations
	initLighting( );
	glEnable (GL_AUTO_NORMAL);
	glEnable (GL_NORMALIZE);
	
}


void	NcSimulationWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	mViewHandle.setView();
	 
	//Drawing X Y Z 
	glDisable(GL_LIGHTING);
	glLineWidth(1.5);	
	glColor3d(0.0, 1.0, 0.0);
	renderText(0, 60, 0, "X");	
	glColor3d(1.0, 0.0, 0.0);
	renderText(60, 0, 0, "Z");	
	glColor3d(0.0, 0.0, 1.0);
	renderText(0, 0, 60, "Y");	

	glBegin(GL_LINES);
		glColor3d(0.0, 1.0, 0.0);
		glVertex3i(0, 0, 0);
		glVertex3i(0, 50, 0);

		glColor3d(1.0, 0.0, 0.0);
		glVertex3i(0, 0, 0);
		glVertex3i(50, 0, 0);
	
		glColor3d(0.0, 0.0, 1.0);
		glVertex3i(0, 0, 0);
		glVertex3i(0, 0, 50);
	glEnd();	
	glEnable(GL_LIGHTING);


	if(mStartSimulation == true && mBoolForSimControl == true)
	{
		mSimStepMode = NcSimulationController::getSimControllerInstance()->runSimulation();
	}
	else if(mStartSimulation == true && mBoolForSimControl == false)
	{
		glCallList(NcCode::getLastExecutedDL());

		for(int i = 0; i < NcCode::getLastExecutedLocalDLIndex(); i++)
		{
			glCallList(NcCode::getCumulativeDLList().at(i));
		}
	}
	else
	{
		if(mIsNewFile == false)
			NcDisplay::getNcDisplayInstance()->callDeformedBodyDL(
				NcDisplay::getNcDisplayInstance()->getStockDisplayListIndex());
	}
	mViewHandle.resetView();
}


void	NcSimulationWindow::resizeGL(int width, int height)
{
	mViewHandle.resetAspectRatio(width,height);
	updateGL();
	
}
void	NcSimulationWindow::mousePressEvent(QMouseEvent *inEvent)
{
	makeCurrent();
	mMouseDownPoint = inEvent->pos();
	updateGL();
}
void	NcSimulationWindow::mouseReleaseEvent(QMouseEvent *inEvent)
{
	makeCurrent();
	mMouseDownPoint.setX (0);
	mMouseDownPoint.setY (0);
	unsetCursor();
	updateGL();					//Redraw the view
}
void	NcSimulationWindow::mouseMoveEvent(QMouseEvent *inEvent)
{
	makeCurrent();
	if(inEvent->buttons() & Qt::LeftButton) 
		rotate(inEvent->pos());
	else if(inEvent->buttons() & Qt::MidButton)
	{
		setCursor(Qt::ClosedHandCursor);
		pan(inEvent->pos());
	}
	updateGL();					//Redraw the view
}
void	NcSimulationWindow::wheelEvent(QWheelEvent *dtvEvent)
{
	makeCurrent();
	if(dtvEvent->delta()>0)
		zoomIn();
	else
		zoomOut();
	updateGL();					//Redraw the view
}

void	NcSimulationWindow::rotate(QPoint pressPoint)
{
	makeCurrent();
	//Increment the object rotation angles
	mViewHandle.rotate(0.5 *(double)(pressPoint.y() - mMouseDownPoint.y()),
						 0.5 *(double)(pressPoint.x() - mMouseDownPoint.x()));
	mMouseDownPoint=pressPoint;	//Set the mouse point
}
void	NcSimulationWindow::pan(QPoint pressPoint)
{
	makeCurrent();
	mViewHandle.pan(mMouseDownPoint.x(),mMouseDownPoint.y(),
					pressPoint.x(),pressPoint.y());
	mMouseDownPoint=pressPoint;	//Set the mouse point
}
void	NcSimulationWindow::frontView()
{
	makeCurrent();
	mViewHandle.resetAll();updateGL();
}
void	NcSimulationWindow::backView()
{
	makeCurrent();
	mViewHandle.resetAll();mViewHandle.rotate(0,180.0);updateGL();
}
void	NcSimulationWindow::topView()
{
	makeCurrent();
	mViewHandle.resetAll();mViewHandle.rotate(90.0,0.0);updateGL();
}
void	NcSimulationWindow::bottomView()
{
	makeCurrent();
	mViewHandle.resetAll();mViewHandle.rotate(-90.0,0.0);updateGL();
}
void	NcSimulationWindow::lhSideView()
{
	makeCurrent();
	mViewHandle.resetAll();mViewHandle.rotate(0,90.0);updateGL();
}
void	NcSimulationWindow::rhSideView()
{
	makeCurrent();
	mViewHandle.resetAll();mViewHandle.rotate(0,-90.0);updateGL();
}
void	NcSimulationWindow::isometricView()
{
	makeCurrent();
	mViewHandle.resetAll();mViewHandle.rotate(35.26,-45.0);updateGL();
}
void	NcSimulationWindow::zoomout()
{
	makeCurrent();
	glMatrixMode(GL_PROJECTION);

	if(gscale > 1.0)
		gscale = 1.0;
	if(gscale > .1)
		gscale = gscale-0.1;
	else
	{
		gscale=0.1;
		return ;
	}

	glScalef(gscale, gscale, gscale);
	glMatrixMode(GL_MODELVIEW);
	updateGL();
}

void	NcSimulationWindow::zoomin()
{
	makeCurrent();
	if(gscale < 1.0)
		gscale = 1.0;
	gscale = gscale + 0.1;
	glMatrixMode(GL_PROJECTION);
	glScalef(gscale, gscale, gscale);
	glMatrixMode(GL_MODELVIEW);
	updateGL();
}


void	NcSimulationWindow::reinitializeSimControllers()
{
	mStartSimulation = false;
	mBoolForSimControl = false;
	gstepmode = false;
	mSimStepMode = false;
}


void	NcSimulationWindow::setFrontView()
{   
	makeCurrent();
	frontView();
}

void	NcSimulationWindow::setTopView()
{	
	makeCurrent();
	topView();
}

void	NcSimulationWindow::setSideView()
{	
	makeCurrent();
	lhSideView();
}

void	NcSimulationWindow::setIsoView()
{	
	makeCurrent();
	isometricView();
}


bool	NcSimulationWindow::updateSimulationWindow()
{
	makeCurrent();
	mStartSimulation = true;
	gstepmode = false;
	updateGL();
	return mSimStepMode;
}


void	NcSimulationWindow::pause()
{
	mStartSimulation = true;
	//NcSimulationController::getSimControllerInstance()->resetOpCountForDisplay();
}


NcSimulationWindow::ViewHandle::ViewHandle()
{
	mAspectRatio	= 1.0;
	mViewOrtho		= 100;
	mBackgroundDL	= 0;

	resetAll();
	resetAspectRatio(1,1);
}
NcSimulationWindow::ViewHandle::~ViewHandle()
{
	glDeleteLists(mBackgroundDL,1);
}
void NcSimulationWindow::ViewHandle::resetAll()
{
	mDPan[0]		= 0.0;
	mDPan[1]		= 0.0;
	mDPan[2]		= 0.0;
	mAngle[0]		= 0.0;
	mAngle[1]		= 0.0;
	mSpin			= 0; 
}
void NcSimulationWindow::ViewHandle::setView()
{
	glDisable (GL_DEPTH_TEST);											// Enable Depth Testing
	drawBackground();
	glEnable (GL_DEPTH_TEST);											// Enable Depth Testing
	setViewVolume();
	//glPushMatrix();		
	glTranslated(mDPan[0],mDPan[1],mDPan[2]);
	glRotated(mAngle[0],1,0,0);
	glRotated(mAngle[1],0,1,0);
	glRotated(mSpin,0,0,1);
	
	glGetDoublev (GL_MODELVIEW_MATRIX, mMVmatrix);
	glGetDoublev (GL_PROJECTION_MATRIX, mPVmatrix);
}
void NcSimulationWindow::ViewHandle::resetView()
{
	glMatrixMode(GL_MODELVIEW);
	//glPopMatrix();
	
	glFinish();
}
void NcSimulationWindow::ViewHandle::resetAspectRatio(int width, int height)
{
	mAspectRatio = (double)height/(double)width  ;
	mObjectVPSize[0] = width;
	mObjectVPSize[1] = height;

	if(!mBackgroundDL)
		genBackgroundDL();
	resetLight();
}
void NcSimulationWindow::ViewHandle::resetOrtho(double	inViewOrtho)
{
	mViewOrtho = fabs(inViewOrtho);resetLight();
}

void NcSimulationWindow::ViewHandle::setViewVolume()
{
	glViewport (0, 0, mObjectVPSize[0], mObjectVPSize[1]);
	glMatrixMode(GL_PROJECTION);	
	glLoadIdentity ();
	glOrtho(-mViewOrtho,mViewOrtho,-mViewOrtho*mAspectRatio,mViewOrtho*mAspectRatio,-1000*mViewOrtho,1000*mViewOrtho);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
}
void NcSimulationWindow::ViewHandle::rotate(double inX,double inY)
{
	mAngle[0]+=inX;mAngle[1]+=inY;
}

void NcSimulationWindow::ViewHandle::pan(int fromX,int fromY,int toX,int toY)
{
	GLint viewport[4]={0,0,mObjectVPSize[0],mObjectVPSize[1]};
	GLint realy;							/* OpenGL y coordinate position */
	GLdouble wx1, wy1, wz1;					/* returned world x, y, z coords     */
	GLdouble wx2, wy2, wz2; /* returned world x, y, z coords     */

	/* note viewport[3] is height of window in pixels */
	realy = mObjectVPSize[1] - toY - 1;
	gluUnProject ((GLdouble) toX, (GLdouble) realy, 0.5,
		mMVmatrix, mPVmatrix, viewport, &wx1, &wy1, &wz1);

	realy = mObjectVPSize[1] - fromY - 1;
	gluUnProject ((GLdouble) fromX, (GLdouble) realy, 0.5,
		mMVmatrix, mPVmatrix, viewport, &wx2, &wy2, &wz2);

	mDPan[0]+=(wx1-wx2)/1.0;
	mDPan[1]+=(wy1-wy2)/1.0;
	mDPan[2]+=(wz1-wz2)/1.0;
}

void NcSimulationWindow::ViewHandle::drawBackground()
{
	glViewport (0,0, mObjectVPSize[0],mObjectVPSize[1]);
	glMatrixMode (GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity ();	
	glOrtho(-1.0, 1.0, -1.0, 1.0,-1000,1000);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity ();	
	
	glDisable(GL_LIGHTING);
		glCallList(mBackgroundDL);
	glEnable(GL_LIGHTING);

	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void NcSimulationWindow::ViewHandle::resetLight()
{
	GLfloat light0pos[4]= {0.0f*mViewOrtho, 5.0f*mViewOrtho, 10.0f*mViewOrtho, 1.0f};
	GLfloat light1pos[4]= {5.0f*mViewOrtho, 0.0f*mViewOrtho, 10.0f*mViewOrtho, 1.0f};
	GLfloat light2pos[4]= {5.0f*mViewOrtho, 10.0f*mViewOrtho, 5.0f*mViewOrtho, 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1pos);
	glLightfv(GL_LIGHT2, GL_POSITION, light2pos);
}
void NcSimulationWindow::ViewHandle::genBackgroundDL()
{
	mBackgroundDL = glGenLists(1);
    glNewList(mBackgroundDL, GL_COMPILE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin(GL_QUADS);
			glColor3d(0.99, 0.99, 0.99);
		glVertex3d(-1.0, -1.0, 0.0);
		glVertex3d(1.0, -1.0, 0.0);
			glColor3d(0.44, 0.56, 0.89);
		glVertex3d(1.0, 1.0, 0.0);
		glVertex3d(-1.0, 1.0, 0.0);
		glEnd();
		glDisable(GL_POLYGON_OFFSET_FILL);
	glEndList();	
}


void NcSimulationWindow::ViewHandle::updateBackgroundDL(QColor bgcolor)
{
	//glClear()
	mBackgroundDL = glGenLists(1);
	glNewList(mBackgroundDL, GL_COMPILE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_QUADS);
	glColor3d(1,1,1);//
	//glColor3d(0.99, 0.99, 0.99);
	glVertex3d(-1.0, -1.0, 0.0);
	glVertex3d(1.0, -1.0, 0.0);
	glColor3d(bgcolor.red(),bgcolor.green(),bgcolor.blue());
	//glColor3d(0.44, 0.56, 0.89);
	glVertex3d(1.0, 1.0, 0.0);
	glVertex3d(-1.0, 1.0, 0.0);
	glEnd();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEndList();	
}

void NcSimulationWindow::callUpdateBg(QColor colorId)
{
	mViewHandle.updateBackgroundDL(colorId);
}	

