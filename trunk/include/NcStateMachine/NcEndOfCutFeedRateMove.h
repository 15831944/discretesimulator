#ifndef NC_ENDOFCUTFEEDRATEMOVE
#define NC_ENDOFCUTFEEDRATEMOVE

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT NcEndOfCutFeedRateMove : public QObject,  public NcCode
	{
		Q_OBJECT

	public:

		NcEndOfCutFeedRateMove();	
		STATUS		generateDisplayList();
		bool		executeCode(SimulationState simstate, NcCode *code = 0);
		void		reinitializeCode();	

	private:
		int			noOfDL;
		int			mPartProfileIndex;

	signals:
		void		updateCycleTime(int, int, int);
		
	};
}


#endif