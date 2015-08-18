#ifndef NC_AUXRAPIDMOVE
#define NC_AUXRAPIDMOVE

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT NcAuxRapidMove : public QObject,  public NcCode
	{
		Q_OBJECT

	public:
		NcAuxRapidMove(double rapid_f);	
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