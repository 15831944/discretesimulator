#ifndef NC_STARTUPSTOCKDISPLAY
#define NC_STARTUPSTOCKDISPLAY

#include "NcStateMachine\NcCode.h"
#include <QtOpenGL>
namespace DiscreteSimulator
{
	//for displaying the startup stock when first line of nc code file
	//is being executed- to avoid flickering which happens when there is no DL
	//to display and gl window is cleared

	class NCSTATEMACHINEEXPORT NcStartupStockDisplay : public NcCode
	{
	public:
		static NcStartupStockDisplay*	getStockDisplayInstance();
		bool	executeCode(SimulationState simstate, NcCode *code = 0);
		void	setDLForStartupStock(GLuint dlid)	{mStartupStockDLId = dlid;}

	private:
		NcStartupStockDisplay(); 
		~NcStartupStockDisplay();
		GLuint							mStartupStockDLId;
		static	NcStartupStockDisplay	*mStockDisplayInstance;
	};
}

#endif