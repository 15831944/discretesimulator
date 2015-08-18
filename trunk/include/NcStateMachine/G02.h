#ifndef CODE_G02
#define CODE_G02


#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class Profile;

	class NCSTATEMACHINEEXPORT G02 : public QObject,  public NcCode    //circular interpolation CW
	{
		Q_OBJECT

	public:
		G02();	
		inline void		setI(double i);
		inline double	getI();
		inline void		setK(double i);
		inline double	getK();
		STATUS			generateDisplayList();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		bool			executeLastDLForCode();
		void			reinitializeCode();

	protected:
		double I, K;

	private:
		int				noOfDL;
		int				mPartProfileIndex;

	signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);

	};
}
#endif