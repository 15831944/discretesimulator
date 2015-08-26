#ifndef NC_CODE
#define NC_CODE


#include <QObject>
#include <QList>

#include "NcUtility\NcGlobalDefinitions.h"
#include "NcStateMachine\NcStateMachineGlobal.h"


#include <QtOpenGL>
namespace DiscreteSimulator
{
	

	class Profile;
	class NcCodeBlock;

	class NCSTATEMACHINEEXPORT NcCode
	{
	public:
		NcCode();
		virtual ~NcCode();

		virtual void				setX(double x);
		virtual double				getX();
		virtual void				setZ(double z);
		virtual double				getZ();
		virtual void				setF(double f);
		virtual double				getF();
		virtual void				setU(double u);
		virtual double				getU();
		virtual void				setW(double w);
		virtual double				getW();
		virtual void				setI(double);
		virtual double				getI();
		virtual void				setD(double);
		virtual double				getD();
		virtual void				setK(double);
		virtual double				getK();
		virtual void				seta(double);
		virtual double				geta();
		virtual void				setdiaX(double);
		virtual double				getdiaX();
		virtual void				setdiaZ(double);
		virtual double				getdiaZ();
		virtual void				setpitch(double);
		virtual double				getpitch();
		virtual void				setdeltaR(double);
		virtual double				getdeltaR();
		virtual void				setdeltaK(double);
		virtual double				getdeltaK();
		virtual void				setS(int S);
		virtual double				getS();
		virtual void				setstartX(double );
		virtual double				getstartX();
		virtual void				setstartZ(double );
		virtual double				getstartZ();

		virtual void				setCycleStartX(double cyclestartx);
		virtual double				getCycleStartX();

		virtual void				setCycleStartZ(double cyclestartz);
		virtual double				getCycleStartZ();

		virtual void				setAsAuxillaryPath(bool auxillarypath);
		virtual bool				isAuxillaryPath();

		virtual void				generateToolPathsForThecode(){} //added to enable call in G74
		virtual void				initializePath(){}
		
		virtual void				generateCuttingPass(){}
		virtual void				generateApproachPath(){}
		virtual void				generateReturnPath(NcCode *){}
		virtual	void				initializeParameters(){}
		virtual	void				setAsFirstPeck(bool peckstatus){}
	
		virtual void				setT(double);
		virtual double				getT();
		virtual	void				setCutEndXValue(double value){}

		virtual	bool				executeCode(SimulationState simstate, NcCode *code = 0);
		virtual bool				executeLastDLForCode()	{return true;}
		virtual QList<Profile *>*	getCodePartProfileList();

		virtual STATUS				generateDisplayList(){return OK;}

		virtual void				setPreviousNcCode(NcCode *code) {mPreviousCode = code;}

		virtual	void				setAsMachiningCode(bool maccode) {mIsMachiningCode = maccode;}

		virtual bool				isMachiningCode() {return mIsMachiningCode;}

		static	GLuint				getLastExecutedDL() { return mLastExecutedDL; }
		static	GLuint				getLastExecutedLocalDLIndex() { return mLastLocalIndexDL; }

		static QList<GLuint>		getCumulativeDLList() { return mCumulativeDLList; }

		virtual	void				reinitializeCode() {}

	public:
		CODE_Type			ctype;
		CODE_Type			toolType;
		bool				ccoolant1;
		bool				ccoolant2;	
		int					id;
		NcCodeBlock			*mAssociatedCodeBlock;
		

	protected:
		double				X, F, Z, U, D, W;
		int					T;
		double				mSpindleSpeed;
		double				mStartX;			//start X pos for individual cut
		double				mStartZ;
		double				mCycleStartX;		//start X pos for the cycle
		double				mCycleStartZ;
		QList<Profile *>	*mPartProfileList;	//profile kept in code so that code can generate
												//any number of display lists as needed
												//(in case of multi depth cuts)
		bool				mIsAuxillaryPath; 
		double				mCodeExecutionTime;
		NcCode				*mPreviousCode;
		bool				mIsMachiningCode;
		
		static QList<GLuint> mCumulativeDLList;	
		static int			mListIndex;	
		static int			mTotalCycleTime;
		int					mLocalIndex;

		static	GLuint		mLastExecutedDL;
		static	GLuint		mLastLocalIndexDL;	

		int					mHours;
		int					mMinutes;
		int					mSeconds;
	};


	double NCSTATEMACHINEEXPORT cal_angle(double x, double y);


}


#endif