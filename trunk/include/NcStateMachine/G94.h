#ifndef CODE_G94
#define CODE_G94

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class Profile;

	class NCSTATEMACHINEEXPORT G94 : public QObject, public NcCode	//facing cycle
	{		
		Q_OBJECT
		
	public:
		G94();
		virtual ~G94();
		STATUS			generateDisplayList();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		bool			executeLastDLForCode();
		void			reinitializeCode();

	private:
		int			noOfDL;
		int			mPartProfileIndex;
		
				//diff in radii in case of taper
	signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);
	};
}

#endif