#include "NcGeomKernel\NcProfile.h"
#include "NcStateMachine\NcToolController.h"
#include "NcStateMachine\NcAuxRapidMove.h"
#include <QtOpenGL>
#include <iostream>
using namespace std;
using namespace DiscreteSimulator;

NcAuxRapidMove::NcAuxRapidMove(double rapid_f)
{
	F = rapid_f;								//in mm/min or in/min
	mPartProfileList = new QList<Profile *>;
	noOfDL = 0;
	mPartProfileIndex = 0;
}	


void	NcAuxRapidMove::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
}



STATUS	NcAuxRapidMove::generateDisplayList()
{
	mCodeExecutionTime = sqrt((Z - mStartZ) * (Z - mStartZ) + (X - mStartX) * (X - mStartX)) / F;  // mm / (mm/min)  gives time in min

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;	
	
	Profile *gProfile = new Profile();
	gProfile->type = CGAUX;
	gProfile->typeTool = CT00;
	mPartProfileList->push_back(gProfile);

	int n = MAX_LINEAR_SUBDIV;
	int no_pts=(n);
	vector<NcVector> profile(no_pts);
	//profile->P.resize(profile->no_pts);
	/*profile->allocate();*/

	profile[0] = NcVector(mStartZ,mStartX,0);
	//profile->P[0][1] = mStartX;
	//profile->P[0][2] = 0;

	
	profile[n-1] = NcVector(Z,X,0);
	//profile->P[n-1][1] = X;
	//profile->P[n-1][2] = 0;

	double u = 0.0;
	double du = 1.0 / (double)(n-1);

	for(int i=0; i < n-1; i++, u+=du)
	{
			
			profile[i] = profile[0] * (1-u) + profile[n-1] * (u);
	}

	GLuint newlistindex = glGenLists(1);
	/*profile->mAssociated2DDLIndexes->push_back(newlistindex);*/
	gProfile->addProfileDisplayListIndex(newlistindex);
	mCumulativeDLList.push_back(newlistindex);
	mListIndex++;
	mLocalIndex = mListIndex;

	glNewList(newlistindex, GL_COMPILE);	
		
		glColor3d(1.0, 0.0, 0.0);
		glBegin(GL_LINE_STRIP);
			for(int i = 0; i < n; i++)
				glVertex3f(profile[i][0], profile[i][1], profile[i][2]);
		glEnd();

		
	glEndList();
	gProfile->setProfile(profile);
	return OK;
}


bool	NcAuxRapidMove::executeCode(SimulationState simstate, NcCode *code) //return true when code execution is complete else return false
{
	mPreviousCode = code;

	if(simstate == RUNNING)
	{
		if(mPartProfileIndex < mPartProfileList->size())
		{
			if(noOfDL < mPartProfileList->at(mPartProfileIndex)->no_pts)
			{
				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

				mPreviousCode->executeLastDLForCode();
				
				for(int i = 0; i < mLocalIndex; i++)
				{
					glCallList(mCumulativeDLList.at(i));
				}

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

				mPreviousCode->executeLastDLForCode();

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


			mPreviousCode->executeLastDLForCode();

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
					->updateToolPosition(mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][0],
										 mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][1],
										 mPartProfileList->at(mPartProfileList->size() - 1)->P[mPartProfileList->at(mPartProfileList->size() - 1)->no_pts - 1][2]);


		mPreviousCode->executeLastDLForCode();

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

		mPreviousCode->executeLastDLForCode();

		for(int i = 0; i < mLocalIndex; i++)
		{
			glCallList(mCumulativeDLList.at(i));
		}

		return false;
	}
	return false;				
}


