#include "NcStateMachine\F.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
using namespace DiscreteSimulator;

F::F()
{
	connect(this, SIGNAL(updateFeedInStatusWindow(int)), 
		NcToolController::getToolControllerInstance(), SLOT(updateFeedRate(int)));

}

F::~F()
{}

void	F::reinitializeCode()
{
	mPreviousCode = 0;
}

bool F::executeCode(SimulationState simstate, NcCode *code)
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
	emit updateFeedInStatusWindow(NcCode::F);
	return true;
}