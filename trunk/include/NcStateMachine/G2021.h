#ifndef NC_G2021
#define NC_G2021

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT G20 : public NcCode  //English units (inch, feet)
	{
	public:
		/*G20();
		~G20();*/
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		void			reinitializeCode();
	};

	class NCSTATEMACHINEEXPORT G21 : public NcCode	//Metric units (mm, meter)
	{
	public:
		/*G21();
		~G21();*/
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		void			reinitializeCode();
	};

}

#endif