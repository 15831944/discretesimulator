#ifndef CODE_G90
#define CODE_G90

#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{
	class Profile;

	class NCSTATEMACHINEEXPORT G90 : public QObject, public NcCode				//Canned Turning & Boring
	{		
		Q_OBJECT
		
	public:
		G90();
		virtual ~G90();
		inline void		setI(double i);
		inline double	getI();
		STATUS			generateDisplayList();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);
		bool			executeLastDLForCode();
		void			reinitializeCode();		

	private:
		double		I;
		int			noOfDL;
		int			mPartProfileIndex;
		
				//diff in radii in case of taper
	signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);
	};
}

#endif