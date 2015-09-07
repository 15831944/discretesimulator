#include "NcStateMachine\MCode.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
using namespace DiscreteSimulator;

M03::M03()
{
	NcMachine::NcMachineInstance()->mSpindleStatus = SM03;

	connect(this, SIGNAL(updateSpindleStatus(bool)), 
		NcToolController::getToolControllerInstance(), SLOT(updateNcSpindleStatus(bool)));
}

//M03::~M03()
//{}

bool	M03::executeCode(SimulationState simstate, NcCode *code)
{

	if(mPreviousCode)
	{
		mPreviousCode->executeLastDLForCode();
	}
	else
	{
		NcStartupStockDisplay::getStockDisplayInstance()->executeCode(simstate);
	}

	emit updateSpindleStatus(true);
	return true;
}


M05::M05()
{
	NcMachine::NcMachineInstance()->mSpindleStatus = SM05;

	connect(this, SIGNAL(updateSpindleStatus(bool)), 
		NcToolController::getToolControllerInstance(), SLOT(updateNcSpindleStatus(bool)));
}

//M05::~M05()
//{}

bool	M05::executeCode(SimulationState simstate, NcCode *code)
{
	emit updateSpindleStatus(false);
	return true;
}



////M30::M30()
//{}
//
//M30::~M30()
//{}

bool	M30::executeCode(SimulationState simstate, NcCode *code)
{
	mPreviousCode = code;

	if(mPreviousCode)
	{
		NcToolController::getToolControllerInstance()->displayToolAtDwellPos();
		mPreviousCode->executeLastDLForCode();
	}
	else
	{
		NcStartupStockDisplay::getStockDisplayInstance()->executeCode(simstate);
	}
	
	return true;
}

M06::M06() //constructor for class M06 this function join the s
{
	//connect(this,SIGNAL(updateTool(bool)),NcToolController::getToolControllerInstance(),SLOT());

}
//M06::~M06()
//{
//
//}

bool DiscreteSimulator::M06::executeCode( SimulationState simstate, NcCode *code /*= 0*/ )
{
	emit updateTool(true);
	return true;
}

M07::M07()
{

}

//M07::~M07() 
//{
//
//}


M08::M08()
{
	connect(this,SIGNAL(updateCoolantStatus(bool)),NcToolController::getToolControllerInstance(),SLOT(updateNCCoolantStatus(bool)));

}
//
//M08::~M08()
//{
//
//}

bool DiscreteSimulator::M08::executeCode( SimulationState simstate, NcCode *code /*= 0*/ )
{
	emit updateCoolantStatus(true);
	return true;
}

M09::M09()
{
	connect(this,SIGNAL(updateCoolantStatus(bool)),NcToolController::getToolControllerInstance(),SLOT(updateNCCoolantStatus(bool)));

}

//M09::~M09()
//{
//
//}

bool DiscreteSimulator::M09::executeCode( SimulationState simstate, NcCode *code /*= 0*/ )
{
	emit updateCoolantStatus(false);
	return false;
}
