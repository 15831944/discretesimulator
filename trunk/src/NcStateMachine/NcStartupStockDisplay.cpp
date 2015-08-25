#include "NcStateMachine\NcStartupStockDisplay.h"

using namespace DiscreteSimulator;

NcStartupStockDisplay	*NcStartupStockDisplay::mStockDisplayInstance = 0;


NcStartupStockDisplay*	NcStartupStockDisplay::getStockDisplayInstance()
{
	if(mStockDisplayInstance == 0)
	{
		mStockDisplayInstance = new NcStartupStockDisplay();
	}
	return mStockDisplayInstance;
}


//NcStartupStockDisplay::NcStartupStockDisplay()
//{}
//
//NcStartupStockDisplay::~NcStartupStockDisplay()
//{}


bool	NcStartupStockDisplay::executeCode(SimulationState simstate, NcCode *code)
{
	glCallList(mStartupStockDLId);
	return true;
}