#ifndef CODE_G00
#define CODE_G00

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class Profile;

	class NCSTATEMACHINEEXPORT G00 : public QObject, public NcCode
	{
		Q_OBJECT

	public:
		G00(double rapid_f);	
		STATUS			generateDisplayList();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		bool			executeLastDLForCode();
		void			reinitializeCode();

	private:
		int			noOfDL;
		int			mPartProfileIndex;

	signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);
	};
}

#endif