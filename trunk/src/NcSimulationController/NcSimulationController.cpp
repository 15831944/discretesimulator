#include "NcStateMachine\NcMachine.h"
#include "NcGeomKernel\NcProfile.h"
#include "NcGeomKernel\NcRevolve.h"
#include "NcGeomKernel\NcPolygonBoolean.h"
#include "NcSimulationController\NcSimulationController.h"
#include "NcUtility\NcVector.h"
#include "NcUtility\NcStlImport.h"
#include "NcStateMachine\NcCode.h"
#include "NcStateMachine\G00.h"
#include "NcStateMachine\G01.h"
#include "NcStateMachine\G02.h"
#include "NcStateMachine\G03.h"

#include "NcStateMachine\G90.h"
#include "NcStateMachine\G74.h"
#include "NcStateMachine\G75.h"
#include "NcStateMachine\NcEndOfCutFeedRateMove.h"
#include "NcStateMachine\NcCodeBlock.h"
#include <iostream>
#include <QMessageBox>

#include <QTextStream>
#include <QChar>

using namespace std;
using namespace DiscreteSimulator;

NcSimulationController	*NcSimulationController::mSimControllerInstance = 0;

int NcSimulationController::executingCodeLineNumber = 0;
int NcSimulationController::executingBlockOfCurrLine = 0;

NcSimulationController::NcSimulationController() 
{
	mNcSimulationMode = BYCYCLE;

	mSimulationState = PAUSED;

	indexOfLastGCodeLine = -1;

	currentcode = 0;

	mLastMachiningCode = 0;
}


NcSimulationController::~NcSimulationController()
{
}

void	NcSimulationController::setSimulationState(SimulationState simulationstate)
{
	mSimulationState = simulationstate;
}

NcSimulationController* NcSimulationController::getSimControllerInstance()
{
	if(0 == mSimControllerInstance)
	{
		mSimControllerInstance = new NcSimulationController();
	}
	return mSimControllerInstance;
}

void	NcSimulationController::setSimulationMode(SimulationMode mode)
{
	mNcSimulationMode = mode;
}

SimulationMode	NcSimulationController::getSimulationMode()
{
	return mNcSimulationMode;
}

bool	NcSimulationController::runSimulation() 
{
	if(mNcSimulationMode == BYBLOCK)
	{
		return runSimulationByBlock();
	}
	else if(mNcSimulationMode == BYCYCLE)
	{
		return runSimulationByCycle();
	}

	return true;
}


bool	NcSimulationController::runSimulationByBlock()  //run simulation in block by block mode
{		
	if(executingCodeLineNumber < NcMachine::NcMachineInstance()->mNcCodeBlockList->size())
	{
		if(executingBlockOfCurrLine < NcMachine::NcMachineInstance()->mNcCodeBlockList->at(executingCodeLineNumber)->mBlockCodeList.size())
		{
			emit codeEditorRequiresUpdate(NcMachine::NcMachineInstance()->mNcCodeBlockList->at(executingCodeLineNumber)->mLineIndex);
			
			//cout << "line no: " << executingCodeLineNumber << " Block No: " << executingBlockOfCurrLine << endl;
			
			currentcode =  NcMachine::NcMachineInstance()->mNcCodeBlockList->at(executingCodeLineNumber)->mBlockCodeList.at(executingBlockOfCurrLine);

			if(currentcode->isMachiningCode() == true)
			{
				//auxillary m/c codes to display the dl of last machining code 
				//since auxillary codes do not perform machining
				mLastMachiningCode = currentcode;	
			}

			if(currentcode->ctype == PRGSTOP)
			{
				return currentcode->executeCode(mSimulationState, mLastMachiningCode);
			}

			if(currentcode->ctype == CGAUX)
			{ 
				//check for mLastMachiningCode = 0 which happens when F and S codes are in the first line of the file
				//and pass code for the default stock display(this code to be created)
				//mLastMachiningCode != 0 if F and S are not in the first line of the file
				if(currentcode->executeCode(mSimulationState, mLastMachiningCode))
				{
					executingBlockOfCurrLine++;
				}		
			}
			else
			{
				if(currentcode->executeCode(mSimulationState))
				{
					executingBlockOfCurrLine++;
				}	
			}

			return false;
		}
		else
		{
			if(currentcode)
			{
				if(currentcode->ctype == CGAUX)
					currentcode->executeCode(mSimulationState, mLastMachiningCode);
				else
					currentcode->executeCode(mSimulationState);
			}
			
			executingCodeLineNumber++;
			executingBlockOfCurrLine = 0;

			return true;
		}
	}
	else
	{
		mSimulationState = END;
		if(indexOfLastGCodeLine == -1)
		{
			//iterate  through code block lists to keep index of the last line number containing the G
			//code to be displayed at the end of simulation
			QList<NcCodeBlock *>::iterator bItr = NcMachine::NcMachineInstance()->mNcCodeBlockList->begin();
			for(; bItr != NcMachine::NcMachineInstance()->mNcCodeBlockList->end(); bItr++)
			{
				if((*bItr)->mContainsMachiningGCode == true)
				{
					indexOfLastGCodeLine = NcMachine::NcMachineInstance()->mNcCodeBlockList->indexOf((*bItr));
				}
			}
		}
		NcCode *gc = NcMachine::NcMachineInstance()->mNcCodeBlockList->at(indexOfLastGCodeLine)->getGCodeInBlock();
		if(gc)
			gc->executeCode(mSimulationState);

		emit enableRewindButton();
	}

	return true;
}

bool	NcSimulationController::runSimulationByCycle()	//run simulation in cycle mode
{
	if(executingCodeLineNumber < NcMachine::NcMachineInstance()->mNcCodeBlockList->size())
	{
		if(executingBlockOfCurrLine < NcMachine::NcMachineInstance()->mNcCodeBlockList->at(executingCodeLineNumber)->mBlockCodeList.size())
		{
			emit codeEditorRequiresUpdate(NcMachine::NcMachineInstance()->mNcCodeBlockList->at(executingCodeLineNumber)->mLineIndex);
			
			//cout << "line no: " << executingCodeLineNumber << " Block No: " << executingBlockOfCurrLine << endl;
			
			currentcode =  NcMachine::NcMachineInstance()->mNcCodeBlockList->at(executingCodeLineNumber)->mBlockCodeList.at(executingBlockOfCurrLine);

			if(currentcode->isMachiningCode() == true)
			{
				//auxillary m/c codes display the dl of last machining code 
				//since auxillary codes do not perform machining
				mLastMachiningCode = currentcode;	
			}

			if(currentcode->ctype == PRGSTOP) //ctype of M30 is PRGSTOP: M30 executes the last DL of the last machining code before the M30 code
			{
				return currentcode->executeCode(mSimulationState, mLastMachiningCode);
			}

			if(currentcode->ctype == CGAUX)		//auxillary codes F,S,T etc contain pointer to the last m/c code so as to display
			{									//the last DL of the last M/c code
				if(currentcode->executeCode(mSimulationState, mLastMachiningCode))
				{
					executingBlockOfCurrLine++;
				}		
			}
			else
			{
				if(currentcode->executeCode(mSimulationState))
				{
					executingBlockOfCurrLine++;
				}	
			}

			return false;
		}
		else
		{
			if(currentcode)
			{
				if(currentcode->ctype == CGAUX)
					currentcode->executeCode(mSimulationState, mLastMachiningCode);
				else
					currentcode->executeCode(mSimulationState);
			}
			
			executingCodeLineNumber++;
			executingBlockOfCurrLine = 0;

			return true;
		}
	}
	else
	{
		mSimulationState = END;
		if(indexOfLastGCodeLine == -1)
		{
			//iterate  through code block lists to keep index of the last line number containing the G
			//code to be displayed at the end of simulation
			QList<NcCodeBlock *>::iterator bItr = NcMachine::NcMachineInstance()->mNcCodeBlockList->begin();
			for(; bItr != NcMachine::NcMachineInstance()->mNcCodeBlockList->end(); bItr++)
			{
				if((*bItr)->mContainsMachiningGCode == true)
				{
					indexOfLastGCodeLine = NcMachine::NcMachineInstance()->mNcCodeBlockList->indexOf((*bItr));
				}
			}
		}
		NcCode *gc = NcMachine::NcMachineInstance()->mNcCodeBlockList->at(indexOfLastGCodeLine)->getGCodeInBlock();
		if(gc)
			gc->executeCode(mSimulationState);

		emit enableRewindButton();
	}

	return true;
}

void	NcSimulationController::rewindSimulation()
{
	executingCodeLineNumber = 0;
	executingBlockOfCurrLine = 0;
	mLastMachiningCode = 0;
	currentcode = 0;
	mSimulationState = RUNNING;
	indexOfLastGCodeLine = -1;

	QList<NcCodeBlock *>::iterator blocklistItr = NcMachine::NcMachineInstance()->mNcCodeBlockList->begin();
	for(; blocklistItr != NcMachine::NcMachineInstance()->mNcCodeBlockList->end(); blocklistItr++)
	{
		QList<NcCode *>::iterator codeItr = (*blocklistItr)->mBlockCodeList.begin();
		for(; codeItr != (*blocklistItr)->mBlockCodeList.end(); codeItr++)
		{
			if((*codeItr)->ctype != CGAUX)
			{
				(*codeItr)->reinitializeCode();
			}
		}
	}

	emit codeEditorRequiresUpdate(NcMachine::NcMachineInstance()->mNcCodeBlockList->at(executingCodeLineNumber)->mLineIndex);	
}



static int currentoperationNumber = 0;
static int currentdlindex = 0;

// //increment currentdlindex for every click of next button on main window
////not possible in the NEXT if case in getCurrentDisplayListIndex()
void	NcSimulationController::setOpIndexForNEXTState()
{
	currentoperationNumber++;
	currentdlindex = 0;
}

