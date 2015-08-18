#ifndef NC_SPEED
#define NC_SPEED

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT S : public QObject, public NcCode
	{
		Q_OBJECT

	public:
		S();
		~S();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		void			reinitializeCode();

	signals:
		void		updateSpeedInStatusWindow(double);
	};
}

#endif