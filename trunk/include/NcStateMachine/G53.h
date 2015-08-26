#ifndef CODE_G53
#define CODE_G53

#include <QObject>
#include "NcStateMachine/NcCode.h"

namespace DiscreteSimulator
{
	class  NCSTATEMACHINEEXPORT G53 : public QObject, public NcCode	
	{
		Q_OBJECT

	public:
		G53();
		virtual ~G53();
		STATUS			generateDisplayList();
		bool			executeCode(SimulationState simstate, NcCode *code = 0);

	private:
		double		I;
		int			noOfDL;
		int			mPartProfileIndex;
signals:
		void		updateToolPathDisplay(int);
		void		updateCycleTime(int, int, int);
	
	};
}
#endif // G53_H
