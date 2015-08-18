#include "NcStateMachine\NcCodeBlock.h"
#include "NcStateMachine\NcCode.h"
#include "NcUtility\NcGlobalDefinitions.h"
using namespace DiscreteSimulator;

NcCodeBlock::NcCodeBlock()
{
	mContainsMachiningGCode = false;
}


NcCode*	NcCodeBlock::getGCodeInBlock()
{
	if(mContainsMachiningGCode == false)
	{
		return 0;
	}
	else
	{
		QList<NcCode *>::iterator itr =	mBlockCodeList.begin();
		for(; itr != mBlockCodeList.end(); itr++)
		{
			if((*itr)->isMachiningCode() == true)	//to avoid getting last machining code as G00(approach path of the canned cycles)
			{										//when simulation reaches END state in simulation controller (fixes a display bug)
				switch((*itr)->ctype)
				{
				case CG00:
				case CG01:
				case CG02:
				case CG03:			
				case CG74:
				case CG75:
				case CG76:
				case CG90:
				case CG92:
				case CG94:
					{
						return (*itr);
					}
				}
			}
		}
	}
	return 0;
}