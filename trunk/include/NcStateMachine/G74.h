#ifndef CODE_G74
#define CODE_G74

#include "NcStateMachine\NcCode.h"
#include"NcUtility\NcVector.h"
#include<vector>

namespace DiscreteSimulator
{
	class Profile;

	class NCSTATEMACHINEEXPORT G74 : public QObject, public NcCode  //peck drilling cycle
	{
		Q_OBJECT

	public:
		G74();	
		inline void		setK(double k);
		inline double	getK();
		STATUS			generateDisplayList();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		/*void			initializePath();*/
		void			generateToolPathsForThecode();
		void			generateCuttingPass();
		void			generateApproachPath();
		void			generateReturnPath(NcCode *code);
		void			initializeParameters();
		void			setAsFirstPeck(bool peckstatus){mIsFirstPeck = peckstatus;}
		bool			executeLastDLForCode();
		void			reinitializeCode();

	protected:
		double			K;					//Depth of cut
		int				noOfDL;
		double			mPreviousZValue;	//used for returning tool to this depth before 
											//cutting the material of depth K / remainder depth
											//equal to end tool position of the previous cut 
	
		double			mCutStartZValue;
		double			mCutEndZValue;
		double			dia;
		int				ptindex;
		std::vector<NcVector> P;
		//double			**P;
		int				noOfKDepthCuts;
		double			remainderDepthToCut;
		double			totaldepthfortool;
		int				mPartProfileIndex;
		bool			mIsFirstPeck;

	signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);
	};
}

#endif