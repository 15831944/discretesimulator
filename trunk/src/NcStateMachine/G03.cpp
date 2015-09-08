#include "NcStateMachine\G03.h"
#include "NcStateMachine\NcToolController.h"
#include "NcGeomKernel\NcProfile.h"
#include <QtOpenGL>
#include <iostream>
using namespace std;
using namespace DiscreteSimulator;

G03::G03()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
	mPartProfileList = new QList<Profile *>;

	connect(this, SIGNAL(updateToolPathDisplay(int)), 
		NcToolController::getToolControllerInstance(), SLOT(updateTPDisplay(int)));

	connect(this, SIGNAL(updateCycleTime(int, int, int)),
		    NcToolController::getToolControllerInstance(), SLOT(updateCycleTime(int, int, int)));
}

void	G03::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0;
}

void G03::setI(double i)
{
	I = i;			
}

double G03::getI()
{
	return I;		
}

void G03::setK(double k)
{
	K = k;		
}

double G03::getK()
{
	return K;		
}


STATUS	G03::generateDisplayList()
{
	Profile *profile = new Profile();
	profile->type = CG03;
	profile->typeTool = CT01;
	mPartProfileList->push_back(profile);

	double cx, cy, x, y;
	double x2, y2;
	double st_angle = 0, end_angle = 0;
	double r;
	double theta, dtheta;
	double arc_pts[MAX_ANG_SUBDIV][2];
	x2 = Z;
	y2 = X;

	profile->no_pts = MAX_ANG_SUBDIV; /* To be modified in future*/
	/*profile->P.resize(MAX_ANG_SUBDIV);*/
	//profile->allocate();

	cx = mStartZ + K;
	cy = mStartX + I;
	r = sqrt((x2 - cx) * (x2 - cx) + (y2 - cy) * (y2 - cy));
	x = mStartZ - cx;
	y = mStartX - cy;
	st_angle = cal_angle(x,y);
	x = x2 - cx;
	y = y2 - cy;

	end_angle = cal_angle(x, y);
	
	if(st_angle > end_angle)
		end_angle = end_angle + 360; 

	dtheta = (end_angle - st_angle) / (double)(MAX_ANG_SUBDIV - 1);

	double arclengthcut = ((end_angle - st_angle) * (NC_PI / 180.0)) * r;
	double feed = F ;
	double sspeed = mSpindleSpeed;
	mCodeExecutionTime =  ( arclengthcut / (F * mSpindleSpeed) ) * 60;

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

	mHours = (int)(mTotalCycleTime / 3600);
	int min = mTotalCycleTime % 3600;
	mMinutes = (int)(min / 60);
	int sec = min % 60;
	mSeconds = sec;
	
	theta = st_angle;

	for(int i = 0; i < MAX_ANG_SUBDIV; theta = theta + dtheta, i++)
	{
		float x, y;
		x = cx + r * cos(theta * NC_PI / 180.0);
		y = cy + r * sin(theta * NC_PI / 180.0);
		arc_pts[i][0] = x;
		arc_pts[i][1] = y;
		profile->P[i][0] = x;
		profile->P[i][1] = y;
		profile->P[i][2] = 0;
	}

	GLuint newlistindex = glGenLists(1);
	profile->mAssociated2DDLIndexes->push_back(newlistindex);

	mCumulativeDLList.push_back(newlistindex);
	mListIndex++;
	mLocalIndex = mListIndex;

	glNewList(newlistindex, GL_COMPILE);
		glColor3d(0,1,0);
	
		for(int i = 1; i < MAX_ANG_SUBDIV; i++)
		{
			glBegin(GL_LINES);
			glVertex3f(arc_pts[i-1][0],arc_pts[i-1][1],0);
			glVertex3f(arc_pts[i][0],arc_pts[i][1],0);
			glEnd();
		}
		
	glEndList();	

	return OK;
}



bool	G03::executeCode(SimulationState simstate, NcCode *code) //return true when code execution is complete else return false
{
	if(simstate == RUNNING)
	{
		if(mPartProfileIndex < mPartProfileList->size())
		{
			if(noOfDL < mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->size())
			{	
				noOfDL++;

				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
										 mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

				noOfDL--;

				glCallList(mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->mAssocitedDBDLIndexes->at(noOfDL);

				for(int i = 0; i < mLocalIndex; i++) //for displaying tool path DL upto the index in this code
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


bool	G03::executeLastDLForCode()
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
