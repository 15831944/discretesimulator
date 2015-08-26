#ifndef CODE_G54
#define CODE_G54

#include <QObject>
#include "NcStateMachine/NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT G54 : public QObject, public NcCode
	{
		Q_OBJECT

	public:
		G54( );
		virtual ~G54();
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
#endif // G54_H
