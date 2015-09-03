#ifndef CODE_G76
#define CODE_G76

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class Profile;

	//multi pass threading cycle
	//I code for taper threading not implemented
	//A code for included angle of insert and P code
	//for infeed method not implemented

	class NCSTATEMACHINEEXPORT G76 : public QObject, public NcCode  
	{
		Q_OBJECT

	public:
		G76();	
		inline void		setK(double k);
		inline double	getK();
		STATUS			generateDisplayList();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		/*void			initializePath();*/
		void			generateToolPathsForThecode();
		void			generateCuttingPass();
		void			generateApproachPath();
		void			generateReturnPath(NcCode *code);
		void			setCutEndXValue(double cutendxvalue);
		bool			executeLastDLForCode();
		void			reinitializeCode();

	protected:
		double			K;	
		int				noOfDL;												
		double			mCutStartXValue;
		double			mCutEndXValue;
		double			equalDepthCuts;
		std::vector<NcVector> P;
	/*	double			**P;*/
		int				noOfIDepthCuts;
		double			remainderDepthToCut;
		double			totaldepthfortool;
		double			mFirstPassDepth;
		int				mPartProfileIndex;
	

	signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);
	};
}

#endif