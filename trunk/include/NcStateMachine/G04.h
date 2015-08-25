#ifndef CODE_G04
#define CODE_G04

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class Profile;

	class NCSTATEMACHINEEXPORT G04 : public QObject,  public NcCode
	{
		Q_OBJECT

	public:
		G04();	
		/*~G04();*/
		void			setDwellTime(int time)
		{
			mDwellTime = time;
			mCodeExecutionTime = mDwellTime;
			mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

			mHours = (int)(mTotalCycleTime / 3600);
			int min = mTotalCycleTime % 3600;
			mMinutes = (int)(min / 60);
			int sec = min % 60;
			mSeconds = sec;	
		}
		bool			executeCode(SimulationState simstate, NcCode *code = 0);

	protected:
		int				mDwellTime;	

	signals:
		void		updateCycleTime(int, int, int);



	};
}

#endif