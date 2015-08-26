#include "NcUIComponents\NcToolPathWindow.h"
#include "NcSimulationController\NcSimulationController.h"
#include "NcDisplay\NcDisplay.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <math.h>
#include <gl/GLU.h>

using namespace DiscreteSimulator;


NcToolPathWindow::NcToolPathWindow(QWidget *parent, QGLWidget *shareWidget) :
QGLWidget(parent, shareWidget)
{
	xRot = 0;
 	yRot = 0;
 	zRot = 0;
	gscale = 1.0;
	mAspectRatio = 2.0;
	setFocusPolicy(Qt::ClickFocus);
	mStartSimulation = false;
}

//NcToolPathWindow::~NcToolPathWindow()
//{
//}

void NcToolPathWindow::setOrthographicProjection()
{
	makeCurrent();
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();
	 
	glOrtho (-170.0, 30.0, -30.0, 170.0, -3000.0, 3000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}


void NcToolPathWindow::initializeGL()
{
	makeCurrent();
	glClearColor(0, 0, 0, 1.0);
   	glEnable (GL_BLEND);
   	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
   	glLineWidth (1.5);
   	glEnable (GL_LINE_SMOOTH);
 	qglClearColor(QColor(0, 0, 0));
 	glShadeModel(GL_SMOOTH);
 	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
}


void NcToolPathWindow::paintGL()
{	
	makeCurrent();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	
	glLineWidth(1.5);	
	glColor3d(1.0, 0.0, 0.0);
	renderText(75,0,0,"Z");		
	glBegin(GL_LINES);
		glVertex3i(0,0,0);
		glVertex3i(50,0,0);
	glEnd();

	glColor3d(0.0, 1.0, 0.0);
	renderText(0,75,0,"X");		
	glBegin(GL_LINES);
		glVertex3i(0,0,0);
		glVertex3i(0,50,0);
	glEnd();

	
	if(mStartSimulation == true)
		NcDisplay::getNcDisplayInstance()->callToolPathDL();

	
	GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        qDebug() << "OpenGL error: " << err << gluErrorString(err)<<  endl;
    }


}

void NcToolPathWindow::resizeGL(int width, int height)
{
	makeCurrent();
	mAspectRatio = (float) width / (float) height;
	glViewport(0, 0, width, height);
	
	setOrthographicProjection();

}

void NcToolPathWindow::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
	offset=QPoint();
}

void NcToolPathWindow::mouseReleaseEvent(QMouseEvent *event)
{
 	if (event->button() == Qt::MidButton)
	{
		makeCurrent();
     	lastPos = QPoint();
        glMatrixMode(GL_PROJECTION);
        glTranslatef(offset.x()/10, offset.y()/10, 0.0);
        glMatrixMode(GL_MODELVIEW);
        updateGL();
   }
}


void NcToolPathWindow::mouseMoveEvent(QMouseEvent *event)
{
 	int dx = event->x() - lastPos.x();
 	int dy = event->y() - lastPos.y();

 	if(event->buttons() & Qt::MidButton)
 	{
    	offset += event->pos() - lastPos;
    	lastPos = event->pos();
 	}

}

void NcToolPathWindow::wheelEvent(QWheelEvent *event)
{
	makeCurrent();
 	int numDegrees = event->delta() / 8;
 	double numSteps = numDegrees / 15.0f;
 	gscale = pow(0.8, numSteps);
    glMatrixMode(GL_PROJECTION);
    glScalef(gscale, gscale, gscale);
    glMatrixMode(GL_MODELVIEW);
    updateGL();
}


void NcToolPathWindow::reinitializeSimVariables()
{
	mStartSimulation = false;
}


void NcToolPathWindow::updateOperation(int u)
{
	makeCurrent();
	op_u = u;
	updateGL();
}

void	NcToolPathWindow::updateToolPathWindow()
{
	makeCurrent();
	mStartSimulation = true;
	updateGL();
}
