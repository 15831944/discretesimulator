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