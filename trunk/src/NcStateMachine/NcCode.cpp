#include <cmath>
#include "NcStateMachine\NcCode.h"

using namespace DiscreteSimulator;
QList<GLuint> NcCode::mCumulativeDLList;

int		NcCode::mListIndex = -1;
GLuint	NcCode::mLastExecutedDL = -1;
GLuint	NcCode::mLastLocalIndexDL = -1;
int	NcCode::mTotalCycleTime = 0;

NcCode::NcCode()
{
	ccoolant1 = false;
	ccoolant2 = false;
	mLocalIndex = -1;
	toolType  = UNKNOWNTOOL;
	mPreviousCode = 0;
	mIsAuxillaryPath = false; //to identify G00 and G01 as paths for the return  motion of tool to the cycle start postition
								// or approach motion to the depth of cut and also to avoid generating deformed body DL for G01

	mIsMachiningCode = false;
}

NcCode::~NcCode()
{

}

void NcCode::setX(double x)
{
	X = x / 2.0;
}
		
double NcCode::getX()
{
	return X;
}

void NcCode::setZ(double z)
{
	Z = z;
}

double NcCode::getZ()
{
	return Z;
}

void NcCode::setF(double f)
{
	F = f;
}

double NcCode::getF()
{
	return F;
}

void NcCode::setU(double u)
{
	U = u;		
}
		
double NcCode::getU()
{
	return U;		
}

void NcCode::setW(double w)
{
	W = w;		
}

double NcCode::getW()
{
	return W;		
}
		
		
void NcCode::setI(double){}
double NcCode::getI(){return 0.0;}
void NcCode::setD(double d)
{
	D = d;
}

double NcCode::getD()
{
	return D;
}
void NcCode::setK(double){}
double NcCode::getK(){return 0.0;}

void NcCode::seta(double){}
double NcCode::geta(){return 0.0;}
void NcCode::setdiaX(double){}
double NcCode::getdiaX(){return 0.0;}
void NcCode::setdiaZ(double){}
double NcCode::getdiaZ(){return 0.0;}
void NcCode::setpitch(double){}
double NcCode::getpitch(){return 0.0;}
void NcCode::setdeltaR(double){}
double NcCode::getdeltaR(){return 0.0;}
void NcCode::setdeltaK(double){}
double NcCode::getdeltaK(){return 0.0;}
void NcCode::setS(int S)
{
	mSpindleSpeed = S;
}

double NcCode::getS()
{
	return mSpindleSpeed;
}

void NcCode::setstartX(double start)
{
	mStartX = start / 2.0;
}

double NcCode::getstartX()
{
	return mStartX;
}

void NcCode::setstartZ(double start)
{
	mStartZ = start;
}

double NcCode::getstartZ()
{
	return mStartZ;
}

void NcCode::setT(double){}
double NcCode::getT(){return 101;}


void	NcCode::setCycleStartX(double cyclestartx)
{
	mCycleStartX = cyclestartx / 2.0;
}

double	NcCode::getCycleStartX()
{
	return mCycleStartX;
}

void	NcCode::setCycleStartZ(double cyclestartz)
{
	mCycleStartZ = cyclestartz;
}

double	NcCode::getCycleStartZ()
{
	return mCycleStartZ;
}

void	NcCode::setAsAuxillaryPath(bool auxillarypath)
{
	mIsAuxillaryPath = auxillarypath;
}

bool	NcCode::isAuxillaryPath()
{
	return mIsAuxillaryPath;
}
	



bool	NcCode::executeCode(SimulationState simstate, NcCode *code)
{
	return true;
}

QList<Profile *>*	NcCode::getCodePartProfileList()
{
	return mPartProfileList;
}

double DiscreteSimulator::cal_angle(double x, double y)
{
	double theta;
	if (x == 0 && y > 0)
		theta = 90;
	else if(x == 0 && y < 0)
		theta = 270;
	else if(y == 0 && x < 0)
		theta = 180;
	else if(y == 0 && x > 0)
		theta = 0;
	else
	{
		theta = std::atan(y / x) * 180 / NC_PI;
		if(x > 0 && theta >= 0.0)
			theta = theta;
		if(x > 0 && theta <= 0.0)
			theta = 360 - std::abs(theta);
		if(x < 0 && theta >= 0.0)
			theta = 180 + std::abs(theta);
		if(x < 0 && theta <= 0.0)
			theta = 180 - std::abs(theta);
	}
	return theta;
}