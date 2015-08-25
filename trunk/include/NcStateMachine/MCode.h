#ifndef NC_MCODE
#define NC_MCODE

#include "NcStateMachine\NcToolController.h"

#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcCode.h"

namespace DiscreteSimulator
{

	/*class NCSTATEMACHINEEXPORT M01 : public NcCode
	{};

	class NCSTATEMACHINEEXPORT M02 : public NcCode
	{};*/

	class NCSTATEMACHINEEXPORT M03 :  public QObject, public NcCode	//spindle on clockwise
	{
		Q_OBJECT
	public:
		M03();
	/*	~M03();*/
		bool	executeCode(SimulationState simstate, NcCode *code = 0);

	signals:
		void	updateSpindleStatus(bool);
	};

	//class M04 : public NcCode	//spindle on clockwise (threading)
	//{};

	class NCSTATEMACHINEEXPORT M05 :  public QObject, public NcCode	//spindle stop
	{
		Q_OBJECT

	public:
		M05();
		/*~M05();*/
		bool	executeCode(SimulationState simstate, NcCode *code = 0);

	signals:
		void	updateSpindleStatus(bool);
	};

	//class M07 : public NcCode	//coolant 1 on
	//{};

	//class M08 : public NcCode	//coolant 2 on
	//{};

	//class M09 : public NcCode	//stop both the coolants
	//{};	
	//
	class M30 :  public QObject,  public NcCode	//end of program
	{
		Q_OBJECT

	public:
		/*M30();*/
		/*~M30();*/
		bool	executeCode(SimulationState simstate, NcCode *code = 0);

	};

	//class M98 : public NcCode	//start of subroutine
	//{};

	//class M99 : public NcCode	//end of subroutine
	//{};
}


#endif