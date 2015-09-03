#include "NcStateMachine\NcMachine.h"
#include "NcStateMachine\G00.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcCodeBlock.h"
#include "NcStateMachine\NcAuxRapidMove.h"
#include "NcGeomKernel\NcProfile.h"
#include "NcStateMachine\G76.h"
using namespace DiscreteSimulator;


G76::G76()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
	mPartProfileList = new QList<Profile *>;
	connect(this, SIGNAL(updateToolPathDisplay(int)), 
		    NcToolController::getToolControllerInstance(), SLOT(updateTPDisplay(int)));

	connect(this, SIGNAL(updateCycleTime(int, int, int)),
		    NcToolController::getToolControllerInstance(), SLOT(updateCycleTime(int, int, int)));
}

/*void	G76::initializePath()
{}*///no code written

void G76::setK(double k)
{
	K = k;		
}

double G76::getK()
{
	return K;
}

void	G76::setCutEndXValue(double cutendxvalue)
{
	mCutEndXValue = cutendxvalue;
}

void	G76::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
}

void	G76::generateToolPathsForThecode()
{
	double p = F / 2.0;							//half pitch

	//time in seconds taking units as threading length = mm, feedrate = mm/rev
	//spindle speed = rev / min
	double threadinglength = fabs(fabs(Z) - fabs(mStartZ));
	double feed = F ;
	double sspeed = mSpindleSpeed;
	//gives turning time in minutes ---> has been converted to seconds 
	
	double timeforonethreadpass =  ( threadinglength / (p * mSpindleSpeed) ) * 60;

	mCodeExecutionTime = timeforonethreadpass * 6; /*currently no of passes is fixed to 6*/

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

	mHours = (int)(mTotalCycleTime / 3600);
	int min = mTotalCycleTime % 3600;
	mMinutes = (int)(min / 60);
	int sec = min % 60;
	mSeconds = sec;

	mFirstPassDepth = X + (K * 2) - (D * 2);

	mCutEndXValue = mFirstPassDepth;

	equalDepthCuts = (mFirstPassDepth - X) / 5 /*temporary number of cuts after the first cut*/; 
	
	generateApproachPath();
	generateCuttingPass();						
	generateReturnPath(this);					
	
	for(int j = 0; j < 5; j++)
	{
		NcCode *newcode = new G76();
		newcode->ctype = CG76;
		newcode->toolType = CT07;
		 
		newcode->setstartX(mStartX * 2);			
		newcode->setstartZ(mStartZ);	

		newcode->setK(K);				
		newcode->setD(D);

		newcode->setX((mCutEndXValue - equalDepthCuts) * 2);
		newcode->setZ(Z);
		newcode->setCutEndXValue(mCutEndXValue - equalDepthCuts);

		mCutEndXValue = mCutEndXValue - equalDepthCuts;
		
		newcode->setF(F);
		newcode->setS(mSpindleSpeed);
		newcode->mAssociatedCodeBlock = mAssociatedCodeBlock;
		
		newcode->generateApproachPath();

		NcMachine::NcMachineInstance()->pushbackGCodeInLists(newcode); 
		
		newcode->generateCuttingPass();
		newcode->generateReturnPath(newcode);		
	}
}

STATUS	G76::generateDisplayList()
{
	QList<Profile *>::iterator profilelistItr = mPartProfileList->begin();
	for(; profilelistItr != mPartProfileList->end(); profilelistItr++)
	{
		GLuint newlistindex = glGenLists(1);
		(*profilelistItr)->mAssociated2DDLIndexes->push_back(newlistindex);

		mCumulativeDLList.push_back(newlistindex);
		mListIndex++;
		mLocalIndex = mListIndex;

		mLastLocalIndexDL = mLocalIndex;

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

void	G76::generateApproachPath()
{
	G00 *rapidmove = new G00(1000);
	rapidmove->ctype = CG00;
	
	rapidmove->setstartX(mCycleStartX); 
	rapidmove->setstartZ(mCycleStartZ); 

	rapidmove->setX(mCutEndXValue);			
	rapidmove->setZ(mCycleStartZ);

	rapidmove->setAsAuxillaryPath(true);
	
	mAssociatedCodeBlock->mBlockCodeList.push_back(rapidmove);
	NcMachine::NcMachineInstance()->pushbackGCodeInLists(rapidmove);

}

void	G76::generateCuttingPass()
{
	Profile *profile = new Profile();
	profile->type = CG76;
	profile->typeTool = CT07;
	mPartProfileList->push_back(profile);

	double X_dist = mStartZ;
		
	double p = F / 2.0;							//half pitch
	int max_threads = (int)fabs((Z - mStartZ) / F);
	
	profile->no_pts = 3 + (max_threads * 2 + 1);
	//profile->allocate();

	profile->P[0][0] = mStartZ;
	profile->P[0][1] = mStartX;
	profile->P[0][2] = 0;

	profile->P[1][0] = mStartZ - p;
	profile->P[1][1] = mCutEndXValue;
	profile->P[1][2] = 0;

	profile->P[max_threads * 2 + 1][0] = Z;
	profile->P[max_threads * 2 + 1][1] = mCutEndXValue;
	profile->P[max_threads * 2 + 1][2] = 0;

	profile->P[1 + (max_threads * 2 + 1)][0] = Z;
	profile->P[1 + (max_threads * 2 + 1)][1] = mStartX;
	profile->P[1 + (max_threads * 2 + 1)][2] = 0;

	profile->P[2 + (max_threads * 2 + 1)][0] = mStartZ;
	profile->P[2 + (max_threads * 2 + 1)][1] = mStartX;
	profile->P[2 + (max_threads * 2 + 1)][2] = 0;

	for(int i = 1; i < (max_threads * 2 + 1); i++)
	{
		X_dist = X_dist - p; 
		profile->P[i][0] = X_dist;
	}

	for(int i = 1; i < (max_threads * 2 + 1); i++)
	{
		if(i % 2 == 1)
			profile->P[i][1] = mCutEndXValue;
		else
			profile->P[i][1] = mStartX;
	}

	for(int i = 1; i < (max_threads * 2 + 1); i++)
	{
		profile->P[i][2] = 0;
	}
	
}

void	G76::generateReturnPath(NcCode *code)
{
	//first rapid move (vertical move)  from (Z, mCutEndXValue) to (Z, mStartX)
	//this retract should be along two axes(inclined) for good finish on the thread -- for future enhancement
	NcAuxRapidMove *rapidmove = new NcAuxRapidMove(1000);
	rapidmove->ctype = CGAUX;
	
	rapidmove->setstartX(mCutEndXValue * 2 );				
	rapidmove->setstartZ(Z);					

	rapidmove->setX(mStartX * 2);			
	rapidmove->setZ(Z);

	rapidmove->setF(1000);
	rapidmove->setS(mSpindleSpeed);

	rapidmove->setAsAuxillaryPath(true);

	mAssociatedCodeBlock->mBlockCodeList.push_back(rapidmove);
	NcMachine::NcMachineInstance()->pushbackGCodeInLists(rapidmove);

	
	//second rapid move (horizontal move) from (Z, mStartX) to (mStartZ, mStartX)	
	NcAuxRapidMove *rapidmove2 = new NcAuxRapidMove(1000);
	rapidmove2->ctype = CGAUX;
	
	rapidmove2->setstartX(mStartX * 2);				
	rapidmove2->setstartZ(Z);					

	rapidmove2->setX(mStartX * 2);			
	rapidmove2->setZ(mStartZ);

	rapidmove2->setF(1000);
	rapidmove2->setS(mSpindleSpeed);

	rapidmove2->setAsAuxillaryPath(true);

	mAssociatedCodeBlock->mBlockCodeList.push_back(rapidmove2);
	NcMachine::NcMachineInstance()->pushbackGCodeInLists(rapidmove2);

}

bool	G76::executeCode(SimulationState simstate, NcCode *code)
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

bool	G76::executeLastDLForCode()
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
		