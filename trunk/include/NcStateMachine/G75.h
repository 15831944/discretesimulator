#ifndef CODE_G75
#define CODE_G75

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class Profile;

	//parting and grooving cycle

	class NCSTATEMACHINEEXPORT G75 : public QObject, public NcCode
	{
		Q_OBJECT

	public:
		G75();
			
	public:
		inline void		setI(double i);
		inline double	getI();
		STATUS			generateDisplayList();
		void			generateCuttingPass();
		void			initializeParameters();
		void			generateReturnPath(NcCode *);
		void			generateToolPathsForThecode();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		void			setCutEndXValue(double value);
		bool			executeLastDLForCode();
		void			reinitializeCode();

	protected:
		double I;		//Depth of cut

		double			mCutEndXValue;
		double			totaldepthfortool;
		double			noOfIDepthCuts;
		double			remainderDepthToCut;
		int				noOfDL;
		int				mPartProfileIndex;
		bool			mIsFirstPeck;

	signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);
	};
}

#endif