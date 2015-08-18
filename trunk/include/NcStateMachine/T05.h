#ifndef NC_T05
#define NC_T05

#include "NcStateMachine\NcStateMachineGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT T05 : public NcCode
	{
	public:
		T05();
		~T05();
		static	void	setToolXOffset(double xoffset)	{mToolXOffset = xoffset;}
		static	void	setToolZOffset(double zoffset)	{mToolZOffset = zoffset;}

	private:
		//X and Z offsets for the tool
		static	double		mToolXOffset;
		static	double		mToolZOffset;


	};
}


#endif