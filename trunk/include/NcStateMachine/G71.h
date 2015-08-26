#ifndef CODE_G71
#define CODE_G71


#include <QObject>
#include "NcStateMachine/NcCode.h"

namespace DiscreteSimulator
{
	class NCSTATEMACHINEEXPORT G71 : public QObject, public NcCode	
	{
		Q_OBJECT

	public:
		G71( );
		virtual ~G71();

	private:
	
	};
}
#endif // G71_H
