#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcToolImporter.h"

#include <gl\GLU.h>

using namespace DiscreteSimulator;

NcToolController*	NcToolController::mToolControllerInstance = 0;


NcToolController::NcToolController()
{
	mCurrentToolXPos = NcMachine::NcMachineInstance()->mToolChangeXPos;
	mCurrentToolYPos = 0;
	mCurrentToolZPos = NcMachine::NcMachineInstance()->mToolChangeZPos;

	mToolStartXPos = NcMachine::NcMachineInstance()->mToolChangeXPos;
	mToolStartYPos = 0;
	mToolStartZPos = NcMachine::NcMachineInstance()->mToolChangeZPos;
	
	 ToolImporter = new NcToolImporter();

	mToolsLoaded = false;
	//connect(this,SIGNAL(updateNCCoolantStatus(bool)),NcStatusWindow::StatusWindowInstance(),SLOT(updateNCCoolantStatus()));
}

//NcToolController::~NcToolController()
//{}

NcToolController*	NcToolController::getToolControllerInstance()
{
	if(mToolControllerInstance == 0)
	{
		mToolControllerInstance = new NcToolController();
	}
	return mToolControllerInstance;
}


void	NcToolController::material_tool()
{
	GLfloat mat_ambient[] = {0.2125f, 0.1275f, 0.054f, 1.0f };
	GLfloat mat_specular[] = { 0.393548f, 0.271906f, 0.166721f, 1.0f };
	GLfloat mat_shininess[] = { 0.0f };
	GLfloat mat_diffuse[] = {0.714f, 0.4284f, 0.18144f, .7f };
	GLfloat EmissionSurface[] = { 0.1f, 0.0f, 0.0f, 1.0f };

	/*following function tells OpenGL to take the source (incoming) color and
	multiply the color (the RGB values) by the alpha value*/

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_EMISSION, EmissionSurface);
}



void NcToolController::display_toolholder(double X, double Y, double Z)
{
	/*
	GLUquadric *sphere = gluNewQuadric();
	glPushMatrix();
	glTranslated(X,Y,Z);
	gluSphere(sphere,10,10,20);
	glPopMatrix();

	return; 
	*/
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	material_tool();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glTranslatef(X, Y, Z);
	
	switch(mCurrentToolType)
	{
	case CT01:
		{
			glRotated(90,0,1,0);
			glRotated(90,0,0,1);
			glScaled(0.5,0.5,0.5);
			glCallList(ToolImporter->getDLIdForTurningTool());
			break;
		}
	case CT02:
		{
			glCallList(ToolImporter->getDLIdForDrillingTool());
			break;
		}
	case CT04:
		{
			glCallList(ToolImporter->getDLIdForThreadingTool());
			break;
		}
	case CT03:
		{
			glRotated(90,0,1,0);
			glRotated(90,0,0,1);
			glRotated(90,0,1,0);
			glScaled(0.5,0.5,0.5);
			glCallList(ToolImporter->getDLIdForPartingTool());
			break;
		}
	default:
		break;
	}
	glDisable(GL_CULL_FACE);
   	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
}



void NcToolController::updateToolPosition(double x, double y, double z)
{	
	mCurrentToolXPos =  y;
	mCurrentToolYPos =	0;
	mCurrentToolZPos =  x - 6;

	display_toolholder(mCurrentToolZPos, mCurrentToolXPos, mCurrentToolYPos);		

	emit updateToolPosInStatusWindow(mCurrentToolZPos, mCurrentToolXPos, mCurrentToolYPos);
}

void	NcToolController::displayToolAtDwellPos()
{
	display_toolholder(mCurrentToolZPos, mCurrentToolXPos, mCurrentToolYPos);	
}


void NcToolController::updateFeedRate(int feed)  //pass feed update signal to main window through tool controller
{
	emit updateFeedRateInStatusWindow(feed);
}

void NcToolController::updateSpeed(double speed) //pass speed update signal to main window through tool controller
{
	emit updateSpeedInStatusWindow(speed);
}

void NcToolController::updateNcSpindleStatus(bool status)
{
	emit updateSpindleStat(status);
}

void NcToolController::updateTPDisplay(int index) //pass tool position update signal to main window through tool controller
{
	emit updateToolPathDisplayInTPWin(index);
}


void	NcToolController::updateCycleTime(int hr, int min, int sec)
{
	emit updateCycleTimeStatus(hr, min, sec);
}

void NcToolController::changeTool(CODE_Type tooltype)
{
	if(mToolsLoaded == false)
	{
		ToolImporter->importLatheTools();
		mToolsLoaded = true;
	}

	if(tooltype == CT01)
		setCurrentToolType(CT01);

	if(tooltype == CT02)
		setCurrentToolType(CT02);
	
	if(tooltype == CT03)
		setCurrentToolType(CT03);

	if(tooltype == CT04)
		setCurrentToolType(CT04);

	display_toolholder(mCurrentToolZPos, mCurrentToolXPos, mCurrentToolYPos);	
	emit updateToolPosInStatusWindow(mCurrentToolZPos, mCurrentToolXPos, mCurrentToolYPos);
}

void DiscreteSimulator::NcToolController::updateNCCoolantStatus( bool status)
{
	emit updateNCCoolantStat(status);
}
/*********************************Pranit****************************************/
//This function is added for updating the tool status to reference position
// This function is called when M06 block code is called.

void DiscreteSimulator::NcToolController::updateRefToolPosition( bool status )
{
	//updateToolPosition(0.0,0.0,0.0);
}