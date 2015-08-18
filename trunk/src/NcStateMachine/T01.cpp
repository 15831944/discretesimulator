#include "NcStateMachine\T01.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
using namespace DiscreteSimulator;

double	T01::mToolXOffset = 0.0;
double	T01::mToolZOffset = 0.0;


T01::T01()
{}

T01::~T01()
{}


bool	T01::executeCode(SimulationState simstate, NcCode *code)
{
	mPreviousCode = code;
	
	NcToolController::getToolControllerInstance()->changeTool(this->toolType);

	if(mPreviousCode)
	{
		mPreviousCode->executeLastDLForCode();
	}
	else
	{
		NcStartupStockDisplay::getStockDisplayInstance()->executeCode(simstate);
	}

	
	return true;
}