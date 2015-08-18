#ifndef NCCODEBLOCK
#define NCCODEBLOCK

#include <QList>
#include <QString>
#include "NcStateMachine\NcStateMachineGlobal.h"


namespace DiscreteSimulator
{
	class NcCode;

	//gathers all commands in a single block, helps in simulating nc code block by block
	//individual code blocks can then be combined to form cycles to run code cycle by cycle
	class NCSTATEMACHINEEXPORT NcCodeBlock  
	{
	public:
		NcCodeBlock();

		NcCode*				getGCodeInBlock(); //return Gcode if mContainsGCode == true else 0;
		
		QString				mCodeBlockString;
		QList<NcCode *>		mBlockCodeList;
		bool				mContainsMachiningGCode;
		int					mLineIndex;


	};
}


#endif