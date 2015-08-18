#include "NcStateMachine\T07.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
using namespace DiscreteSimulator;

double	T07::mToolXOffset = 0.0;
double	T07::mToolZOffset = 0.0;

T07::T07()
{}

T07::~T07()
{}

bool	T07::executeCode(SimulationState simstate, NcCode *code)
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

	NcToolController::getToolControllerInstance()->changeTool(this->toolType);
	return true;
}