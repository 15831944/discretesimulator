#ifndef CODE_G01
#define CODE_G01

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class Profile;

	class NCSTATEMACHINEEXPORT G01 : public QObject, public NcCode
	{
		Q_OBJECT

	public:
		G01();	
		STATUS		generateDisplayList();
		bool		executeCode(SimulationState simstate, NcCode *code = 0);
		bool		executeLastDLForCode();
		void		reinitializeCode();

	private:
		int			noOfDL;
		int			mPartProfileIndex;

	signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);
	};
}

#endif