#include "NcStateMachine/G91.h"
#include <cmath>
#include <iostream>

#include "NcGeomKernel\NcProfile.h"
#include "NcStateMachine\NcToolController.h"
#include <QtOpenGL>
#include <QList>

using namespace DiscreteSimulator;

using namespace std;
// constructor for class G91 .

/***********************************Pranit****************************************/
G91::G91()
{
	I = 0;
	noOfDL = 0;
	mPartProfileIndex = 0;
	mPartProfileList = new QList<Profile *>;
	mXposition = 0;
	mYposition = 0;
	mZposition = 0;
	connect(this,SIGNAL(updateToolPathDisplay(int)),NcToolController::getToolControllerInstance(),SLOT(updateTPDisplay()));

	connect(this,SIGNAL(updateCycleTime(int,int,int)),NcToolController::getToolControllerInstance(),SLOT(updateCycleTime(int,int,int)));
}

//Desructor for class G91
G91::~G91()
{}

//this function initializes the member veriables
void G91::reinitializeCode()
{
	noOfDL = 0;
	mPartProfileIndex = 0 ; 
	mXposition = 0;
	mYposition = 0;
	mZposition = 0;
}

//This is setter method for member veriable I
void G91::setI(double i)
{
	I = i;
}

//This function returns the value of the member veriable I
double G91::getI()
{
	return I;
}

//This function does the necessary calculations and genetates the display list
STATUS G91::generateDisplayList()
{
	Profile* gProfile = new Profile();
	gProfile->type = CG91;
	gProfile->typeTool = CT01;
	mPartProfileList->push_back(gProfile);
	//time in seconds taking units as turning length = mm, feedrate = mm/rev
	//spindle speed = rev / min
	
	double turninglength = fabs(fabs(Z) - fabs(mStartZ));
	double feed = F ;
	double sspeed = mSpindleSpeed;
	
	//gives turning time in minutes ---> has been converted to seconds 
	mCodeExecutionTime =  ( turninglength / (F * mSpindleSpeed) ) * 60;

	mTotalCycleTime = mTotalCycleTime + mCodeExecutionTime;

	mHours = (int)(mTotalCycleTime / 3600);
	int min = mTotalCycleTime % 3600;
	mMinutes = (int)(min / 60);
	int sec = min % 60;
	mSeconds = sec;


	int no_pts= ( 2 + MAX_LINEAR_SUBDIV);
	//profile->allocate();
	vector<NcVector> profile(no_pts);
	profile[0] = NcVector(mStartZ,mStartX,0);
	//profile->P[0][1] = mStartX;
	//profile->P[0][2] = 0;

	profile[1] = NcVector(mStartZ, X + I, 0);
	//profile->P[1][1] = X + I;
	//profile->P[1][2] = 0;

	profile[MAX_LINEAR_SUBDIV] = NcVector(Z,X,0);
	//profile->P[MAX_LINEAR_SUBDIV][1] = X;
	//profile->P[MAX_LINEAR_SUBDIV][2] = 0;

	double u = 0.0;
	double du = 1.0 / (double)(MAX_LINEAR_SUBDIV - 1);

	for(int i = 1; i < MAX_LINEAR_SUBDIV; i++, u += du)
	{
		
			profile[i] = profile[1] * (1-u) + profile[MAX_LINEAR_SUBDIV] * (u);
	}

	profile[1 + MAX_LINEAR_SUBDIV] = NcVector(Z,mStartX,0);
	/*profile->P[1 + MAX_LINEAR_SUBDIV][1] = mStartX;
	profile->P[1 + MAX_LINEAR_SUBDIV][2] = 0;*/

	GLuint newlistindex = glGenLists(1);
	/*profile->mAssociated2DDLIndexes->push_back(newlistindex);*/
	gProfile->addProfileDisplayListIndex(newlistindex);
	mCumulativeDLList.push_back(newlistindex);
	mListIndex++;
	mLocalIndex = mListIndex;

	mLastLocalIndexDL = mLocalIndex;

	glNewList(newlistindex, GL_COMPILE);
	for(int i = 0; i < no_pts - 1; i++)
	{

		glColor3d(0.0, 0.0, 1.0);		//turning color
		glBegin(GL_LINES);
		glVertex3f(profile[i][0], profile[i][1], 0.0); 
		glVertex3f(profile[i+1][0], profile[i+1][1], 0.0); 		
		glEnd();

	}
	glEndList();
	gProfile->setProfile(profile);
	//handle opengl error here 
	
	return OK;
}
bool	G91::executeCode(SimulationState simstate, NcCode *code) //return true when code execution is complete else return false
{
	if(simstate == RUNNING)
	{
		/*cout << "G90 profile index: " << mPartProfileIndex << endl;*/
		if(mPartProfileIndex < mPartProfileList->size())
		{
			if(noOfDL < mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLsize())
			{
				/*cout << "G90 DL no in noOfDL before executing: " << noOfDL << endl;*/

				noOfDL++; //to solve the problem of tool lagging behind the cut in simulation

				//display the tool DL first to avoid disappearance of the tool
				NcToolController::getToolControllerInstance()
					->updateToolPosition(mPartProfileList->at(mPartProfileIndex)->P[noOfDL][0],
					mPartProfileList->at(mPartProfileIndex)->P[noOfDL][1],
					mPartProfileList->at(mPartProfileIndex)->P[noOfDL][2]);

				noOfDL--;	//to solve the problem of tool lagging behind the cut in simulation

				glCallList(mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL));

				mLastExecutedDL = mPartProfileList->at(mPartProfileIndex)->getAssocitedDBDLIndexes(noOfDL);

				for(int i = 0; i < mLocalIndex; i++)
				{
					glCallList(mCumulativeDLList.at(i));
				}

				emit updateToolPathDisplay(mLocalIndex);
				noOfDL++;

				/*cout << "G90 DL no in noOfDL after executing: " << noOfDL << endl;*/
				return false;
			}
			else
			{
				--noOfDL;
				/*cout << "G90 DL no in noOfDL else before: " << noOfDL << endl;*/

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
				/*cout << "G90 DL no in noOfDL else after: " << noOfDL << endl;*/
				return false;
			}
		}
		else
		{
			/*cout << "G90 End of profile list: " << endl;*/

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

bool	G91::executeLastDLForCode()
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

/*********************************************************************************************/