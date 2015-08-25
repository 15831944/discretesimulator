#include "NcStateMachine\T02.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
using namespace DiscreteSimulator;

double	T02::mToolXOffset = 0.0;
double	T02::mToolZOffset = 0.0;
//
//T02::T02()
//{}
//
//T02::~T02()
//{}

bool	T02::executeCode(SimulationState simstate, NcCode *code)
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