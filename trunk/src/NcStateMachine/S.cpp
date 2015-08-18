#include "NcStateMachine\S.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
using namespace DiscreteSimulator;

S::S()
{
	connect(this, SIGNAL(updateSpeedInStatusWindow(double)), 
		NcToolController::getToolControllerInstance(), SLOT(updateSpeed(double)));
}

S::~S()
{}

void	S::reinitializeCode()
{
	mPreviousCode = 0;
}

bool S::executeCode(SimulationState simstate, NcCode *code)
{
	mPreviousCode = code;

	if(mPreviousCode)
	{
		mPreviousCode->executeLastDLForCode();
	}
	else
	{
		NcStartupStockDisplay::getStockDisplayInstance()->executeCode(simstate);
	}

	emit updateSpeedInStatusWindow(mSpindleSpeed);
	return true;
}