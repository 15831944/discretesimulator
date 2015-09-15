#include "NcStateMachine\G01.h"
#include "NcStateMachine\NcToolController.h"
#include "NcGeomKernel\NcProfile.h"
#include <QtOpenGL>
#include <iostream>
using namespace std;
using namespace DiscreteSimulator;

G01::G01()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
	mPartProfileList = new QList<Profile *>;

	connect(this, SIGNAL(updateToolPathDisplay(int)), 
		NcToolController::getToolControllerInstance(), SLOT(updateTPDisplay(int)));

	connect(this, SIGNAL(updateCycleTime(int, int, int)),
		    NcToolController::getToolControllerInstance(), SLOT(updateCycleTime(int, int, int)));
}

void	G01::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
}

STATUS	G01::generateDisplayList()
{
	//time in seconds taking units as turning length = mm, feedrate = mm/rev
	//spindle speed = rev / min
	double turninglength = fabs(fabs(Z) - fabs(mStartZ));
	double feed = F ;
	double sspeed = mSpindleSpeed;
	mCodeExecutionTime =  ( turninglength / (F * mSpindleSpeed) ) * 60;

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

	mHours = (int)(mTotalCycleTime / 3600);
	int min = mTotalCycleTime % 3600;
	mMinutes = (int)(min / 60);
	int sec = min % 60;
	mSeconds = sec;

	Profile *gProfile = new Profile();
	gProfile->type = CG01;
	if(mIsAuxillaryPath == true)
		gProfile->typeTool = CT00;
	else if(mIsAuxillaryPath == false)
		gProfile->typeTool = CT01;

	mPartProfileList->push_back(gProfile);
	
	int n = MAX_LINEAR_SUBDIV;
	vector<NcVector> profile(n);
	/*profile->P.resize(n);
	profile->no_pts = n;*/
	//profile->allocate();

	profile[0]= NcVector(mStartZ,mStartX,0);
	/*profile->P[0][1] = mStartX;
	profile->P[0][2] = 0;*/

	profile[n-1]=NcVector(Z,X,0);
	/*profile->P[n-1][1] = X;
	profile->P[n-1][2] = 0;*/

	double u = 0.0;
	double du = 1.0 / (double)(n-1);

	for(int i=0; i < n-1; i++, u+=du)
	{
			
			profile[i] = profile[0] * (1-u) + profile[n-1] * (u);
	}

	GLuint newlistindex = glGenLists(1);
	//profile->mAssociated2DDLIndexes->push_back(newlistindex);
	gProfile->addProfileDisplayListIndex(newlistindex);
	mCumulativeDLList.push_back(newlistindex);
	mListIndex++;
	mLocalIndex = mListIndex;

	glNewList(newlistindex, GL_COMPILE);	
		glColor3d(0.0, 1.0, 0.0);
		
		
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < n; i++)
				glVertex3f(profile[i][0], profile[i][1], profile[i][2]);
		glEnd();

	
	glEndList();
	gProfile->setProfile(profile);
	return OK;
}

bool	G01::executeCode(SimulationState simstate, NcCode *code) //return true when code execution is complete else return false
{
	if(simstate == RUNNING)
	{
		if(mPartProfileIndex < mPartProfileList->size())
		{
			if(noOfDL < mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLsize())
			{				
				/*noOfDL++;*/
		
				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

				/*noOfDL--;*/
				
				glCallList(mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL);
				
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
					->updateToolPosition(mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][0],
										 mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][1],
										 mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][2]);


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
		NcToolController::getToolControllerInstance()
					->updateToolPosition(mCycleStartZ, mCycleStartX, 0);


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


bool	G01::executeLastDLForCode()
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


