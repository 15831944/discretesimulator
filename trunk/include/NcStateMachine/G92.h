#ifndef CODE_G92
#define CODE_G92

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT G92 : public QObject, public NcCode  //single pass threading cycle
	{
		Q_OBJECT

	public:
		G92();	
		void			setpitch(double f);
		double			getpitch();
		STATUS			generateDisplayList();

		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		bool			executeLastDLForCode();
		void			reinitializeCode();

	private:
		int				noOfDL;
		int				mPartProfileIndex;
		double			pitch;

	signals:
		void			updateToolPathDisplay(int);
		void			updateCycleTime(int, int, int);
		
	};
}

#endif