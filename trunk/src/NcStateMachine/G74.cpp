#include "NcStateMachine\NcCodeBlock.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\NcAuxRapidMove.h"
#include "NcStateMachine\G00.h"
#include "NcStateMachine\G74.h"
#include "NcGeomKernel\NcProfile.h"
#include <iostream>
using namespace std;

using namespace DiscreteSimulator;

G74::G74()		//peck drilling cycle
{
	X = 0.0;
	U = 0.0;
	noOfDL = 0;
	dia = 18.0;
	mPartProfileIndex = 0;
	ptindex = 0;
	mIsFirstPeck = true;
	mPartProfileList = new QList<Profile *>;
	

	connect(this, SIGNAL(updateToolPathDisplay(int)), 
		NcToolController::getToolControllerInstance(), SLOT(updateTPDisplay(int)));

	connect(this, SIGNAL(updateCycleTime(int, int, int)),
		    NcToolController::getToolControllerInstance(), SLOT(updateCycleTime(int, int, int)));
}

void	G74::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
}


void G74::setK(double k)
{
	K = k;		
}

double G74::getK()
{
	return K;
}

void G74::initializeParameters()
{
	mPreviousZValue = mStartZ;
	mCutStartZValue = mStartZ;
	mCutEndZValue = mStartZ - K;
}


void G74::generateToolPathsForThecode()
{
	mPreviousZValue = mStartZ;
	mCutStartZValue = mStartZ;
	mCutEndZValue = mStartZ - K;

	//k = depth of each cut starting from the cycle start position (mStartX and mStartZ)
	//Z = total depth of the hole

	totaldepthfortool = fabs(Z) - fabs(mStartZ);

	noOfKDepthCuts = (int)(totaldepthfortool / K);
	
	//////////////////////////
	//time for integral number of cuts each of length = k
	double timeforoneintegralpass =  ( K / (F * mSpindleSpeed) ) * 60;
	double timeforKIntegralpasses = timeforoneintegralpass * noOfKDepthCuts;
	//////////////////////////
	
	//final depth the tool takes
	remainderDepthToCut = totaldepthfortool - (K * noOfKDepthCuts); //last peck for the remainder depth to be cut

	/////////////////////////
	//time for the final cut of length = remainderDepthToCut
	double timeforfinalcut = ( remainderDepthToCut / (F * mSpindleSpeed) ) * 60;
	////////////////////////

	//total cycle time
	mCodeExecutionTime  = timeforKIntegralpasses + timeforfinalcut;

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

	mHours = (int)(mTotalCycleTime / 3600);
	int min = mTotalCycleTime % 3600;
	mMinutes = (int)(min / 60);
	int sec = min % 60;
	mSeconds = sec;
	P.reserve(10);
	initializePath();
	generateCuttingPass();					//generate first depth of cut
	generateReturnPath(this);				//first return path
	
	for(int j = 0; j < noOfKDepthCuts - 1; j++)
	{
		generateApproachPath();
		NcCode *newcode = new G74();
		newcode->ctype = CG74;
		NcMachine::NcMachineInstance()->pushbackGCodeInLists(newcode);  //push back the first G74 code
						 

		newcode->setCycleStartX(mCycleStartX);
		newcode->setCycleStartZ(mCycleStartZ);

		newcode->setstartX(mStartX);			//end of motion set by the last Gcode( probably a positioning command)
		newcode->setstartZ(mCutEndZValue);	

		newcode->setK(K);						//k represents depth of each cut in drilling in G74 cycle

		newcode->setX(mStartX);
		newcode->setZ(mCutEndZValue - K);

		newcode->setF(F);
		newcode->setS(mSpindleSpeed);

		newcode->toolType = CT02;
		newcode->mAssociatedCodeBlock = mAssociatedCodeBlock;
		newcode->initializeParameters();
		newcode->initializePath();
		
		
		newcode->generateCuttingPass();
		newcode->generateReturnPath(newcode);

		mCutEndZValue = mCutEndZValue - K;
	}

	if(remainderDepthToCut != 0)
	{
		mCutEndZValue = mCutEndZValue - remainderDepthToCut;
		generateApproachPath();
		NcCode *newcode = new G74();
		newcode->ctype = CG74;
		NcMachine::NcMachineInstance()->pushbackGCodeInLists(newcode);  //push back the first G74 code
						 

		newcode->setCycleStartX(mCycleStartX);
		newcode->setCycleStartZ(mCycleStartZ);

		newcode->setstartX(mStartX);		//end of motion set by the last Gcode( probably a positioning command)
		newcode->setstartZ(mCutEndZValue);	

		newcode->setK(K);						//k represents depth of each cut in drilling in G74 cycle

		newcode->setX(mStartX);
		newcode->setZ(mCutEndZValue);

		newcode->setF(F);
		newcode->setS(mSpindleSpeed);

		newcode->toolType = CT02;
		newcode->mAssociatedCodeBlock = mAssociatedCodeBlock;
		newcode->initializeParameters();
		newcode->initializePath();
		
		generateCuttingPass();
		generateReturnPath(newcode);
		
	}
}


void G74::initializePath()
{ P.resize(2);
	/*P = new double* [2];

    for(int i = 0; i < 2; i++)
	{
    	P[i] = new double [3];
    }*/
}


STATUS	G74::generateDisplayList()
{	
	Profile *gProfile = new Profile();
	gProfile->type = CG74;
	gProfile->typeTool = CT02;
	mPartProfileList->push_back(gProfile);

	int no_pts = 2;
	 vector<NcVector> profile(no_pts);
	 profile=P;
	GLuint newlistindex = glGenLists(1);
	/*profile->mAssociated2DDLIndexes->push_back(newlistindex);*/
	gProfile->addProfileDisplayListIndex(newlistindex);
	mCumulativeDLList.push_back(newlistindex);
	mListIndex++;
	mLocalIndex = mListIndex;

	glNewList(newlistindex, GL_COMPILE);
		for(int i = 0; i < no_pts - 1; i += 2)
		{
			
			glColor3d(0.0, 1.0, 1.0);				//1,1,0 cyan color
			glBegin(GL_LINES);
				glVertex3f(profile[i][0], profile[i][1], 0.0); 
				glVertex3f(profile[i+1][0], profile[i+1][1], 0.0); 		
			glEnd();
			
		}
	glEndList();
	gProfile->setProfile(profile);
	return OK;
}


void G74::generateApproachPath()	//to be used after generating the first cut and first return pass
{									//so that the mCutEndZValue is correct
	G00 *rapidmove = new G00(1000);
	rapidmove->ctype = CG00;
	
	rapidmove->setstartX(mCycleStartX); 
	rapidmove->setstartZ(mCycleStartZ); 

	rapidmove->setX(mStartX);			
	rapidmove->setZ(mCutEndZValue);

	rapidmove->setAsAuxillaryPath(true);
	
	mAssociatedCodeBlock->mBlockCodeList.push_back(rapidmove);
	NcMachine::NcMachineInstance()->pushbackGCodeInLists(rapidmove);
}


void G74::generateCuttingPass()
{
	P[0][0] = mCutEndZValue;
	P[0][1] = mStartX;
	P[0][2] = 0;

	P[1][0] = mCutEndZValue;
	P[1][1] = mStartX + dia / 2.0;
	P[1][2] = 0;	
}


void G74::generateReturnPath(NcCode *code)
{
	NcAuxRapidMove *rapidmove = new NcAuxRapidMove(1000);
	rapidmove->ctype = CGAUX;
	
	rapidmove->setstartX(mStartX);									//initialized to tool change pos for first rapid move
	rapidmove->setstartZ(mCutEndZValue);							//subsequent codes should update the cycle start x and z positions

	rapidmove->setX(mCycleStartX);			
	rapidmove->setZ(mCycleStartZ);

	rapidmove->setF(1000);
	rapidmove->setS(mSpindleSpeed);

	rapidmove->setAsAuxillaryPath(true);

	rapidmove->setPreviousNcCode(code);

	mAssociatedCodeBlock->mBlockCodeList.push_back(rapidmove);		//run with the complete block when the simulation is run by block
	NcMachine::NcMachineInstance()->pushbackGCodeInLists(rapidmove);
}


bool	G74::executeCode(SimulationState simstate, NcCode *code)	//return true when code execution is complete else return false
{
	if(simstate == RUNNING)
	{
		if(mPartProfileIndex < mPartProfileList->size())
		{
			if(noOfDL < mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLsize())
			{	
				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

				glCallList(mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL);

				for(int i = 0; i < mLocalIndex; i++)
				{
					glCallList(mCumulativeDLList.at(i));
				}
				
				emit updateToolPathDisplay(mLocalIndex);

				noOfDL+=2;				

				return false;
			}
			else
			{
				noOfDL -= 2;

				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 2][0],
										 mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 2][1],
										 mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 2][2]);

				glCallList(mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL);

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
					->updateToolPosition(mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 2][0],
										 mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 2][1],
										 mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 2][2]);

			glCallList(mPartProfileList->at(mPartProfileList->size() - 1)
						->getAssocitedDBDLIndexes(mPartProfileList->at(mPartProfileList->size() - 1)
						->getAssocitedDBDLsize() - 1));

			mLastExecutedDL = mPartProfileList->at(mPartProfileList->size() - 1)
								->getAssocitedDBDLIndexes(mPartProfileList->at(mPartProfileList->size() - 1)
								->getAssocitedDBDLsize() - 1);

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
		NcToolController::getToolControllerInstance()->updateToolPosition(mCycleStartZ, mCycleStartX, 0);

		glCallList(mPartProfileList->at(mPartProfileList->size() - 1)
						->getAssocitedDBDLIndexes(mPartProfileList->at(mPartProfileList->size() - 1)
						->getAssocitedDBDLsize() - 1));

		mLastExecutedDL = mPartProfileList->at(mPartProfileList->size() - 1)
						->getAssocitedDBDLIndexes(mPartProfileList->at(mPartProfileList->size() - 1)
						->getAssocitedDBDLsize() - 1);

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

		glCallList(mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL));

		mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL);

		for(int i = 0; i < mLocalIndex; i++)
		{
			glCallList(mCumulativeDLList.at(i));
		}

		return false;
	}
	return false;
}


bool	G74::executeLastDLForCode()
{
	glCallList(mPartProfileList->at(mPartProfileList->size() - 1)
				->getAssocitedDBDLIndexes(mPartProfileList->at(mPartProfileList->size() - 1)
				->getAssocitedDBDLsize() -1));

	for(int i = 0; i < mLocalIndex; i++)  
	{
		glCallList(mCumulativeDLList.at(i));
	}

	//used in simulation controller when simulation is paused so that last executed DL is displayed continuously
	mLastExecutedDL = mPartProfileList->at(mPartProfileList->size() - 1)
						->getAssocitedDBDLIndexes(mPartProfileList->at(mPartProfileList->size() - 1)
						->getAssocitedDBDLsize() -1);


	return true;
}


