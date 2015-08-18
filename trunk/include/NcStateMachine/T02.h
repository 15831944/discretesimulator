#ifndef NC_T02
#define NC_T02

#include "NcStateMachine\NcStateMachineGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT T02 : public NcCode
	{
	public:
		T02();
		~T02();
		static	void	setToolXOffset(double xoffset)	{mToolXOffset = xoffset;}
		static	void	setToolZOffset(double zoffset)	{mToolZOffset = zoffset;}
		bool			executeCode(SimulationState simstate, NcCode *code = 0);

	private:
		//X and Z offsets for the tool
		static	double		mToolXOffset;
		static	double		mToolZOffset;

	};
}


#endif