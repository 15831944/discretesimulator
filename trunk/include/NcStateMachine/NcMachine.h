#ifndef NC_STATEMACHINE
#define NC_STATEMACHINE

#include <QList>
#include <QString>
#include "NcUtility\NcGlobalDefinitions.h"
#include "NcStateMachine\NcStateMachineGlobal.h"


namespace DiscreteSimulator
{

	class NcCode;
	class NcCodeBlock;
	
	enum	DimensionUnit {MM, INCH};

	enum	DimensionReference {ABSOLUTE, INCREMENTAL};

	enum	OperationalMode {RAPIDMOTION, LINEARINTERPOL, CWCIRCULARINTERPOL, CCWCIRCULARINTERPOL, DWELL,
							 CANNEDTURNING, PECKDRILLING, PARTINGGROOVING, FACING, SINGLEPASSTHREADING,
							 MULTIPASSTHREADING, MACHINEZERORETURN, INCHUNITMODE, MMUNITMODE};

	enum	ToolType {TURNINGTOOL, DRILLINGTOOL, PARTINGTOOL, THREADINGTOOL, GROOVINGTOOL, BORINGTOOL, FACINGTOOL};

	enum	MCodeType {SPINDLECW, SPINDLECWT, SPINDLESTOP, PROGRAMSTOP};

	class NCSTATEMACHINEEXPORT NcMachine							//class representing the modal cnc state machine
	{
	public:
		virtual ~NcMachine();
		static	NcMachine* NcMachineInstance();

		void	setMachineDimensionUnit(DimensionUnit unit);
		void	setMachineDimensionReferece(DimensionReference dimensionreference);
		
		void	setEndOfMotionXPosition(double xpos);
		void	setEndOfMotionZPosition(double zpos);

		void	setMachineOperationalMode(OperationalMode operationalmode);

		void	setConversionFactor(DimensionUnit unit);

		void	buildGCodeList();
		void	buildMCodes();
		void	buildTCodes();
		void	buildFCodes();
		void	buildSCodes();

		inline void	pushbackGCodeInLists(NcCode *newcode);

		void	initializeCodeBlockInTheMachine(QString codeblock, int linecount);

		friend class cleanupNcMachineInstance;

	public:
		double					mLastMachineXPos;
		double					mLastMachineZPos;
		double					mEndOfMotionX;
		double					mEndOfMotionZ;
		double					mRapidSpeedForX;
		double					mRapidSpeedForZ;
		double					mCurrentSpindleSpeed;
		double					mCurrentFeedRate;
		double					mToolChangeXPos;
		double					mToolChangeZPos;
		double					mCycleStartXPos;
		double					mCycleStartZPos;
		int						mDwellTime;
		DimensionUnit			mDimensionUnit;
		DimensionReference		mDimensionReference;
		OperationalMode			mOperationalMode;
		QList<NcCode *>			*mGCodeList;
		QList<NcCode *>			*mCompleteCodeList;	//contains all the codes in the file. To be executed when simulation is run.
		
		QList<NcCodeBlock *>	*mNcCodeBlockList;
		
		ToolType				mCurrentToolType;
		double					I;
		double					K;
		double					D;
		SpindleStatus			mSpindleStatus;
		MCodeType				mCurrentMCode;
		double					mConversionFactor;

	private:
		NcMachine();
		NcMachine(const NcMachine &);
		NcMachine& operator=(const NcMachine &);
		

	private:
		void	generateInitialRapidMoveToCutDepth(NcCode *newcode);
		void	generateEndOfCutFeedrateMove(NcCode *code);
		void	generateRapidReturnToCycleStart(NcCode *code);
		void	generateG00CodesForG28Code(NcCode *code);

	private:
		NcCodeBlock				*mCurrentCodeBlock;
		static NcMachine		*mNcMachineInstance;
	};
}



#endif