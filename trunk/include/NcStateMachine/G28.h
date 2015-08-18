#ifndef NC_MACHINEZERORETURN
#define NC_MACHINEZERORETURN

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{

	//machine zero return code --- X and Z co-ordinates in the code specifies 
	//the co-ordinates of the intermediate point. G28 should always be associated
	//with at least one axis co-ordinate code otherwise it is an error
	//-----to be implemented in syntax and error checking

	class NCSTATEMACHINEEXPORT G28 : public QObject, public NcCode
	{
		Q_OBJECT

	public:
		G28();
		~G28();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);

	};
}

#endif