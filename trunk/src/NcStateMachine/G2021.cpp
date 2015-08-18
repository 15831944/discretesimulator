#include "NcStateMachine\G2021.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
using namespace DiscreteSimulator;

G20::G20()	//english unit, inch
{}

G20::~G20()
{}

void	G20::reinitializeCode()
{
	mPreviousCode = 0;
}

bool G20::executeCode(SimulationState simstate, NcCode *code)
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
	return true;
}


G21::G21()	//metric unit, mm, meter
{}

G21::~G21()
{}

void	G21::reinitializeCode()
{
	mPreviousCode = 0;
}



bool G21::executeCode(SimulationState simstate, NcCode *code)
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
	return true;
}