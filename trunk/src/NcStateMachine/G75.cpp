#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\G00.h"
#include "NcStateMachine\G01.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcCodeBlock.h"
#include "NcStateMachine\NcAuxRapidMove.h"
#include "NcGeomKernel\NcProfile.h"
#include "NcStateMachine\G75.h"
#include <QtOpenGL>
#include <iostream>
using namespace std;

using namespace DiscreteSimulator;

G75::G75()
{
	X = 0.0;
	U = 0.0;
	noOfDL = 0;
	mPartProfileIndex = 0;
	mIsFirstPeck = true;
	mPartProfileList = new QList<Profile *>;

	connect(this, SIGNAL(updateToolPathDisplay(int)), 
		NcToolController::getToolControllerInstance(), SLOT(updateTPDisplay(int)));

	connect(this, SIGNAL(updateCycleTime(int, int, int)),
		    NcToolController::getToolControllerInstance(), SLOT(updateCycleTime(int, int, int)));
}

void	G75::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
}


void G75::setI(double i)
{	
	I = i;		
}

double G75::getI()
{
	return I;
}


void G75::initializeParameters()
{
	mCutEndXValue = mStartX - I;
}


void G75::setCutEndXValue(double value)
{
	mCutEndXValue = value;
}

void G75::generateToolPathsForThecode()
{
	mCutEndXValue = mStartX - I;

	//I = depth of each cut starting from the cycle start position (mStartX and mStartZ)
	//X = diameter upto which the groove needs to be cut

	totaldepthfortool = fabs(mStartX) - fabs(X);  //diff between start and end positions for the cycle

	noOfIDepthCuts = (int)(totaldepthfortool / I);


	//////////////////////////
	//time for integral number of cuts each of length = k
	double timeforoneintegralpass =  ( I / (F * mSpindleSpeed) ) * 60;
	double timeforIIntegralpasses = timeforoneintegralpass * noOfIDepthCuts;
	//////////////////////////
		
	//final depth the tool takes
	remainderDepthToCut = totaldepthfortool - (I * noOfIDepthCuts); //last peck for the remainder depth to be cut

	/////////////////////////
	//time for the final cut of length = remainderDepthToCut
	double timeforfinalcut = ( remainderDepthToCut / (F * mSpindleSpeed) ) * 60;
	////////////////////////

	//total cycle time
	mCodeExecutionTime  = timeforIIntegralpasses + timeforfinalcut;

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

	mHours = (int)(mTotalCycleTime / 3600);
	int min = mTotalCycleTime % 3600;
	mMinutes = (int)(min / 60);
	int sec = min % 60;
	mSeconds = sec;

	
	generateCuttingPass();						//first cutting pass
	generateReturnPath(this);					//first return pass
	
	for(int j = 0; j < noOfIDepthCuts - 1/*-1 bcoz the first cut is already over*/; j++)
	{
		NcCode *newcode = new G75();
		newcode->ctype = CG75;
		newcode->toolType = CT03;
		NcMachine::NcMachineInstance()->pushbackGCodeInLists(newcode);  //push back the second G75 code for second depth
						 
		newcode->setstartX(mStartX * 2);			//end of motion set by the last Gcode( probably a positioning command)
		newcode->setstartZ(mStartZ);	

		newcode->setI(I);						//I represents depth of each cut in drilling in G75 cycle

		newcode->setX((mCutEndXValue - I) * 2);
		newcode->setZ(mStartZ);

		newcode->setF(F);
		newcode->setS(mSpindleSpeed);

		newcode->mAssociatedCodeBlock = mAssociatedCodeBlock;
	
		newcode->setCutEndXValue(mCutEndXValue - I);
		newcode->generateCuttingPass();
		newcode->generateReturnPath(newcode);		

		mCutEndXValue = mCutEndXValue - I;
	}

	if(remainderDepthToCut != 0)
	{
		mCutEndXValue = mCutEndXValue - remainderDepthToCut;
		
		NcCode *newcode = new G75();
		newcode->ctype = CG75;
		newcode->toolType = CT03;
		NcMachine::NcMachineInstance()->pushbackGCodeInLists(newcode);  //push back the second G75 code for second depth
						 
		newcode->setstartX(mStartX * 2);		
		newcode->setstartZ(mStartZ);	

		newcode->setI(remainderDepthToCut);	

		newcode->setX((mCutEndXValue) * 2);
		newcode->setZ(mStartZ);

		newcode->setF(F);
		newcode->setS(mSpindleSpeed);

		newcode->mAssociatedCodeBlock = mAssociatedCodeBlock;
	
		newcode->setCutEndXValue(mCutEndXValue - remainderDepthToCut);
		newcode->generateCuttingPass();
		newcode->generateReturnPath(newcode);
		/*mCutEndXValue = mCutEndXValue - I;*/
	}

	
	
}



void G75::generateCuttingPass()
{
	Profile *profile = new Profile();
	profile->type = CG75;
	profile->typeTool = CT03;
	mPartProfileList->push_back(profile);

	int num_pass = 10;
	
	double pass = (mStartX - mCutEndXValue) /(double)(num_pass);

	profile->no_pts= num_pass * 2;
	//profile->allocate();
	
	double init_pass = pass;	

	for(int i = 0; i < num_pass * 2; i += 2)
	{
		profile->P[i+1][0] = mStartZ;
		profile->P[i+1][1] = mStartX - pass;

		profile->P[i][0] = mStartZ - 5;
		profile->P[i][1] = mStartX - pass;

		pass = pass + init_pass;
	}

	for(int i = 0; i < (num_pass * 2); i++)
	{
		profile->P[i][2] = 0;
	}
}



STATUS	G75::generateDisplayList()
{	
	QList<Profile *>::iterator profilelistItr = mPartProfileList->begin();
	for(; profilelistItr != mPartProfileList->end(); profilelistItr++)
	{
		GLuint newlistindex = glGenLists(1);
		(*profilelistItr)->mAssociated2DDLIndexes->push_back(newlistindex);

		mCumulativeDLList.push_back(newlistindex);
		mListIndex++;
		mLocalIndex = mListIndex;

		glNewList(newlistindex, GL_COMPILE);
		for(int i = 0; i < (*profilelistItr)->no_pts - 1; i += 2)
		{
			
			glColor3d(1.0, 0.5, 0.5);
			glBegin(GL_LINES);
				glVertex3f((*profilelistItr)->P[i][0], (*profilelistItr)->P[i][1], 0.0); 
				glVertex3f((*profilelistItr)->P[i+1][0], (*profilelistItr)->P[i+1][1], 0.0); 		
			glEnd();
			
		}
		glEndList();
	}
	return OK;
}



void G75::generateReturnPath(NcCode *code)
{
	NcAuxRapidMove *rapidmove = new NcAuxRapidMove(1000);
	rapidmove->ctype = CGAUX;
	
	rapidmove->setstartX(mCutEndXValue * 2);				
	rapidmove->setstartZ(mStartZ);					

	rapidmove->setX(mStartX * 2);			
	rapidmove->setZ(mStartZ);

	rapidmove->setF(1000);
	rapidmove->setS(mSpindleSpeed);

	rapidmove->setAsAuxillaryPath(true);

	mAssociatedCodeBlock->mBlockCodeList.push_back(rapidmove);		//run with the complete block when the simulation is run by block
	NcMachine::NcMachineInstance()->pushbackGCodeInLists(rapidmove);
}


bool	G75::executeCode(SimulationState simstate, NcCode *code) //return true when code execution is complete else return false
{
	if(simstate == RUNNING)
	{
		if(mPartProfileIndex < mPartProfileList->size())
		{
			if(noOfDL < mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->size())
			{
				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

				glCallList(mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL);

				for(int i = 0; i < mLocalIndex; i++)
				{
					glCallList(mCumulativeDLList.at(i));
				}
				
				emit updateToolPathDisplay(mLocalIndex);
				
				noOfDL++;
				return false;
			}
			else
			{
				--noOfDL;
				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 1][0],
										 mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 1][1],
										 mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 1][2]);

				glCallList(mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL);

				for(int i = 0; i < mLocalIndex; i++)
				{
					glCallList(mCumulativeDLList.at(i));
				}
				
				mPartProfileIndex++;
				noOfDL = 0;
				return false;
			}
		}
		else
		{
			NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][0],
										 mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][1],
										 mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][2]);


			glCallList(mPartProfileList->at(mPartProfileList->size() - 1)
						->mAssocitedDBDLIndexes->at(mPartProfileList->at(mPartProfileList->size() - 1)
						->mAssocitedDBDLIndexes->size() - 1));

			mLastExecutedDL = mPartProfileList->at(mPartProfileList->size() - 1)
								->mAssocitedDBDLIndexes->at(mPartProfileList->at(mPartProfileList->size() - 1)
								->mAssocitedDBDLIndexes->size() - 1);

			for(int i = 0; i < mLocalIndex; i++)
			{
				glCallList(mCumulativeDLList.at(i));
			}			

			emit updateCycleTime(mHours, mMinutes, mSeconds);

			return true;
		}
	}
	else if(simstate == END)
	{
		NcToolController::getToolControllerInstance()
					->updateToolPosition(mCycleStartZ, mCycleStartX, 0);

		glCallList(mPartProfileList->at(mPartProfileList->size() - 1)
						->mAssocitedDBDLIndexes->at(mPartProfileList->at(mPartProfileList->size() - 1)
						->mAssocitedDBDLIndexes->size() - 1));

		mLastExecutedDL = mPartProfileList->at(mPartProfileList->size() - 1)
						->mAssocitedDBDLIndexes->at(mPartProfileList->at(mPartProfileList->size() - 1)
						->mAssocitedDBDLIndexes->size() - 1);

		for(int i = 0; i < mLocalIndex; i++)
		{
			glCallList(mCumulativeDLList.at(i));
		}
			
		return true;
	}
	else if(simstate == PAUSED)
	{
		NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

		glCallList(mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL));

		mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL);
		
		for(int i = 0; i < mLocalIndex; i++)
		{
			glCallList(mCumulativeDLList.at(i));
		}
		
		return false;
	}

	return false;			
}


bool	G75::executeLastDLForCode()
{
	glCallList(mPartProfileList->at(mPartProfileList->size() - 1)
				->mAssocitedDBDLIndexes->at(mPartProfileList->at(mPartProfileList->size() - 1)
				->mAssocitedDBDLIndexes->size() -1));

	for(int i = 0; i < mLocalIndex; i++)  
	{
		glCallList(mCumulativeDLList.at(i));
	}

	//used in simulation controller when simulation is paused so that last executed DL is displayed continuously
	mLastExecutedDL = mPartProfileList->at(mPartProfileList->size() - 1)
						->mAssocitedDBDLIndexes->at(mPartProfileList->at(mPartProfileList->size() - 1)
						->mAssocitedDBDLIndexes->size() -1);

	return true;
}













