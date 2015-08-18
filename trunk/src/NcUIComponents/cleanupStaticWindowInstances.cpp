#include "NcUIComponents\cleanupStaticWindowInstances.h"
#include "NcUIComponents\NcSimulationWindow.h"
#include "NcUIComponents\NcToolPathWindow.h"
#include "NcUIComponents\NcStatusWindow.h"
using namespace DiscreteSimulator;

cleanupStaticWindowInstances::~cleanupStaticWindowInstances()
{

	delete NcStatusWindow::mStatusWindowInstance;
	NcStatusWindow::mStatusWindowInstance = 0;
}