#include "NcStateMachine\T04.h"
using namespace DiscreteSimulator;

double	T04::mToolXOffset = 0.0;
double	T04::mToolZOffset = 0.0;

T04::T04()
{}

T04::~T04()
{}

bool	T04::executeCode(SimulationState simstate, NcCode *code)
{
	return true;
}