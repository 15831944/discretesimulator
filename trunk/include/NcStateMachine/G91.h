#ifndef CODE_G91
#define CODE_G91

#include <QObject>


#include "NcStateMachine\NcCode.h"
/************************Pranit*************************/
namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT G91 :public QObject,public NcCode
	{
		Q_OBJECT

	public:
		G91();
		~G91();
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
		double      mXposition;
		double      mYposition;
		double      mZposition;

		//diff in radii in case of taper
signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);

	};
}
#endif // G91_H
/*****************************************************************************/