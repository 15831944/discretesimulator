#ifndef NC_FEED
#define NC_FEED

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT F : public QObject, public NcCode
	{
		Q_OBJECT

	public:
		F();
		/*~F();*/
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		void			reinitializeCode();

	signals:
		void		updateFeedInStatusWindow(int);
	};
}

#endif