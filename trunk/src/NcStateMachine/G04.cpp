#include "NcStateMachine\G04.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
using namespace DiscreteSimulator;

G04::G04()
{
	mLocalIndex = mListIndex;
}

G04::~G04()
{}


bool G04::executeCode(SimulationState simstate, NcCode *code)
{
	static int i = 0;

	mPreviousCode = code;
	
	if(i < mDwellTime)
	{
		if(mPreviousCode)
		{
			NcToolController::getToolControllerInstance()->displayToolAtDwellPos();

			mPreviousCode->executeLastDLForCode();

			for(int i = 0; i < mLocalIndex; i++) //display of toolpath in 3d simulation window
			{
				glCallList(mCumulativeDLList.at(i));
			}
		}
		else
		{
			NcToolController::getToolControllerInstance()->displayToolAtDwellPos();
			
			NcStartupStockDisplay::getStockDisplayInstance()->executeCode(simstate);

			for(int i = 0; i < mLocalIndex; i++)
			{
				glCallList(mCumulativeDLList.at(i));
			}
		}
		i += 5;
		return false;
	}
	else
	{
		if(mPreviousCode)
		{
			NcToolController::getToolControllerInstance()->displayToolAtDwellPos();

			mPreviousCode->executeLastDLForCode();

			for(int i = 0; i < mLocalIndex; i++)
			{
				glCallList(mCumulativeDLList.at(i));
			}
		}
		else
		{
			NcToolController::getToolControllerInstance()->displayToolAtDwellPos();

			NcStartupStockDisplay::getStockDisplayInstance()->executeCode(simstate);

			for(int i = 0; i < mLocalIndex; i++)
			{
				glCallList(mCumulativeDLList.at(i));
			}
		}
		i = 0;

		emit updateCycleTime(mHours, mMinutes, mSeconds);
		
		return true;
	}
	
	return true;
}