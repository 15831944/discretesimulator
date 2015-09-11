#include "NcStateMachine\G00.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcStartupStockDisplay.h"
#include "NcGeomKernel\NcProfile.h"
#include <QtOpenGL>
#include <iostream>
using namespace std;
using namespace DiscreteSimulator;

G00::G00(double rapid_f)
{
	F = rapid_f;								//in mm/min or in/min
	mPartProfileList = new QList<Profile *>;
	noOfDL = 0;
	mPartProfileIndex = 0;

	connect(this, SIGNAL(updateToolPathDisplay(int)), 
		NcToolController::getToolControllerInstance(), SLOT(updateTPDisplay(int)));

	connect(this, SIGNAL(updateCycleTime(int, int, int)),
		    NcToolController::getToolControllerInstance(), SLOT(updateCycleTime(int, int, int)));
}	

void	G00::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
}

STATUS	G00::generateDisplayList()
{
	mCodeExecutionTime = sqrt((Z - mStartZ) * (Z - mStartZ) + (X - mStartX) * (X - mStartX)) / (F * mSpindleSpeed);  // mm / (mm/min)  gives time in min

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

	mHours = (int)(mTotalCycleTime / 3600);
	int min = mTotalCycleTime % 3600;
	mMinutes = (int)(min / 60);
	int sec = min % 60;
	mSeconds = sec;

	Profile *profile = new Profile();
	profile->type = CG00;
	profile->typeTool = CT00;
	mPartProfileList->push_back(profile);

	int n = MAX_LINEAR_SUBDIV;
	profile->no_pts = n;
	profile->P.resize(profile->no_pts);
	/*profile->allocate();*/

	profile->P[0][0] = mStartZ;
	profile->P[0][1] = mStartX;
	profile->P[0][2] = 0;

	
	profile->P[n-1][0] = Z;
	profile->P[n-1][1] = X;
	profile->P[n-1][2] = 0;

	double u = 0.0;
	double du = 1.0 / (double)(n-1);

	for(int i=0; i < n-1; i++, u+=du)
	{
		for(int j = 0; j < 3; j++)	
			profile->P[i][j] = profile->P[0][j] * (1-u) + profile->P[n-1][j] * (u);
	}

	GLuint newlistindex = glGenLists(1);
	profile->mAssociated2DDLIndexes->push_back(newlistindex);
	
	mCumulativeDLList.push_back(newlistindex);
	mListIndex++;
	mLocalIndex = mListIndex;

	mLastLocalIndexDL = mLocalIndex;

	glNewList(newlistindex, GL_COMPILE);	
		
		glColor3d(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < n; i++)
				glVertex3f(profile->P[i][0], profile->P[i][1], profile->P[i][2]);
		glEnd();

	
	glEndList();
	
	return OK;
}


bool	G00::executeCode(SimulationState simstate, NcCode *code) //return true when code execution is complete else return false
{
	if(simstate == RUNNING)
	{
		//cout << "G00 profile index: " << mPartProfileIndex << endl;
		
		if(mPartProfileIndex < mPartProfileList->size())
		{
			if(noOfDL < mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->size())
			{
				//cout << "G00 DL no in noOfDL loop before executing: " << noOfDL << endl;

				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

				glCallList(mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL);
				/*
				for(int i = 0; i < mLocalIndex; i++)
				{
					glCallList(mCumulativeDLList.at(i));
				}
				*/
				emit updateToolPathDisplay(mLocalIndex);
								
				noOfDL++;
				//cout << "G00 DL no in noOfDL loop after executing: " << noOfDL << endl;
				return false;
			}
			else
			{
				--noOfDL;
				//cout << "G00 DL no in else for noOfDL before: " << noOfDL << endl;

					NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 1][0],
										 mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 1][1],
										 mPartProfileList->at(mPartProfileIndex)->P[mPartProfileList->at(mPartProfileIndex)->no_pts - 1][2]);


				glCallList(mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL);
				/*
				for(int i = 0; i < mLocalIndex; i++)
				{
					glCallList(mCumulativeDLList.at(i));
				}
				*/
				mPartProfileIndex++;
				noOfDL = 0;
				//cout << "G00 DL no in else for noOfDL after: " << noOfDL << endl;
				return false;
			}
		}
		else
		{
			//cout << "G00 End of Profile list: " << endl;

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

			/*for(int i = 0; i < mLocalIndex; i++)
			{
				glCallList(mCumulativeDLList.at(i));
			}*/

			emit updateCycleTime(mHours, mMinutes, mSeconds);
			
			return true;
		}
	}
	else if(simstate == END)
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

		/*for(int i = 0; i < mLocalIndex; i++)
		{
			glCallList(mCumulativeDLList.at(i));
		}*/
			
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

		/*for(int i = 0; i < mLocalIndex; i++)
		{
			glCallList(mCumulativeDLList.at(i));
		}*/
		return false;
	}

	return false;
}

bool	G00::executeLastDLForCode()
{
	glCallList(mPartProfileList->at(mPartProfileList->size() - 1)
				->mAssocitedDBDLIndexes->at(mPartProfileList->at(mPartProfileList->size() - 1)
				->mAssocitedDBDLIndexes->size() -1));

	/*for(int i = 0; i < mLocalIndex; i++)
	{
		glCallList(mCumulativeDLList.at(i));
	}*/

	mLastExecutedDL = mPartProfileList->at(mPartProfileList->size() - 1)
				->mAssocitedDBDLIndexes->at(mPartProfileList->at(mPartProfileList->size() - 1)
				->mAssocitedDBDLIndexes->size() -1);

	return true;
}


