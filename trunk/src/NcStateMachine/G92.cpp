#include "NcStateMachine\G92.h"
#include "NcStateMachine\NcToolController.h"
#include "NcGeomKernel\NcProfile.h"
#include <QtOpenGL>

using namespace DiscreteSimulator;

G92::G92()  //single pass threading cycle
{	
	noOfDL = 0;
	mPartProfileIndex = 0;
	mPartProfileList = new QList<Profile *>;

	connect(this, SIGNAL(updateToolPathDisplay(int)), 
		NcToolController::getToolControllerInstance(), SLOT(updateTPDisplay(int)));

	connect(this, SIGNAL(updateCycleTime(int, int, int)),
		    NcToolController::getToolControllerInstance(), SLOT(updateCycleTime(int, int, int)));
}

void G92::setpitch(double f)
{
	pitch = f;
}

double G92::getpitch()
{
	return pitch;
}


void	G92::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
}



STATUS	G92::generateDisplayList()
{	
	Profile *profile = new Profile();
	profile->type = CG92;
	profile->typeTool = CT07;
	mPartProfileList->push_back(profile);
	
	double X_dist = mStartZ;
		
	double p = F / 2.0;							//half pitch
	int max_threads = (int)fabs((Z - mStartZ) / F);

	//time in seconds taking units as threading length = mm, feedrate = mm/rev
	//spindle speed = rev / min
	double threadinglength = fabs(fabs(Z) - fabs(mStartZ));
	double feed = F ;
	double sspeed = mSpindleSpeed;
	//gives turning time in minutes ---> has been converted to seconds 
	mCodeExecutionTime =  ( threadinglength / (p * mSpindleSpeed) ) * 60;

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

	mHours = (int)(mTotalCycleTime / 3600);
	int min = mTotalCycleTime % 3600;
	mMinutes = (int)(min / 60);
	int sec = min % 60;
	mSeconds = sec;
	
	profile->no_pts=( 2 + (max_threads * 2 + 1));
	//profile->allocate();

	profile->P[0][0] = mStartZ;
	profile->P[0][1] = mStartX;
	profile->P[0][2] = 0;

	profile->P[1][0] = mStartZ - p;
	profile->P[1][1] = X;
	profile->P[1][2] = 0;

	profile->P[max_threads * 2 + 1][0] = Z;
	profile->P[max_threads * 2 + 1][1] = X;
	profile->P[max_threads * 2 + 1][2] = 0;

	profile->P[1 + (max_threads * 2 + 1)][0] = Z;
	profile->P[1 + (max_threads * 2 + 1)][1] = mStartX;
	profile->P[1 + (max_threads * 2 + 1)][2] = 0;

	/*profile->P[2 + (max_threads * 2 + 1)][0] = mStartZ;
	profile->P[2 + (max_threads * 2 + 1)][1] = mStartX;
	profile->P[2 + (max_threads * 2 + 1)][2] = 0;*/

	for(int i = 1; i < (max_threads * 2 + 1); i++)
	{
		X_dist = X_dist - p; 
		profile->P[i][0] = X_dist;
	}

	for(int i = 1; i < (max_threads * 2 + 1); i++)
	{
		if(i % 2 == 1)
			profile->P[i][1] = X;
		else
			profile->P[i][1] = mStartX;
	}

	for(int i = 1; i < (max_threads * 2 + 1); i++)
	{
		profile->P[i][2] = 0;
	}
	
	GLuint newlistindex = glGenLists(1);
	profile->mAssociated2DDLIndexes->push_back(newlistindex);

	mCumulativeDLList.push_back(newlistindex);
	mListIndex++;
	mLocalIndex = mListIndex;

	mLastLocalIndexDL = mLocalIndex;

	glNewList(newlistindex, GL_COMPILE);
	for(int i = 0; i < profile->no_pts - 1; i++)
	{
		
		glColor3d(1.0, 0.5, 0.5);
		glBegin(GL_LINES);
			glVertex3f(profile->P[i][0], profile->P[i][1], 0.0); 
			glVertex3f(profile->P[i+1][0], profile->P[i+1][1], 0.0); 		
		glEnd();
		
	}
	glEndList();

	return OK;
}

bool	G92::executeCode(SimulationState simstate, NcCode *code)
{
	if(simstate == RUNNING)
	{
		if(mPartProfileIndex < mPartProfileList->size())
		{
			if(noOfDL < mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->size())
			{				
				noOfDL++; //to solve the problem of tool lagging behind the cut in simulation

				//display the tool DL first to avoid disappearance of the tool
				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

				noOfDL--;	//to solve the problem of tool lagging behind the cut in simulation
				
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

bool	G92::executeLastDLForCode()
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