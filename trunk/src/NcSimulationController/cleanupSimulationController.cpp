#include "NcSimulationController\cleanupSimulationController.h"
#include "NcSimulationController\NcSimulationController.h"
using namespace DiscreteSimulator;

cleanupSimulationController::~cleanupSimulationController()
{
	delete NcSimulationController::mSimControllerInstance;
	NcSimulationController::mSimControllerInstance = 0;
}