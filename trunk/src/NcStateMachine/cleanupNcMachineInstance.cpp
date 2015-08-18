#include "NcStateMachine\cleanupNcMachineInstance.h"
#include "NcStateMachine\NcMachine.h"
using namespace DiscreteSimulator;


cleanupNcMachineInstance::~cleanupNcMachineInstance()
{
	delete NcMachine::mNcMachineInstance;
	NcMachine::mNcMachineInstance = 0;
}