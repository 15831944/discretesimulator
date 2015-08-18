#ifndef NCFANUC10T11TFILEREADER
#define NCFANUC10T11TFILEREADER

#include <QObject>
#include <QString>
#include <QList>
#include <QTextStream>
#include "NcFileIO\NcFileIOGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"

class QFile;

namespace DiscreteSimulator
{
	class NCEXPORT NcFanuc10T11TFileReader  : public QObject
	{
		Q_OBJECT

	public:
		static NcFanuc10T11TFileReader*	getReaderInstance();
		~NcFanuc10T11TFileReader();

		void		setCurrentNcFile(QString filename) {mCurrentFileName = filename;}
		QFile*		getCurrentNcFile();
		bool		openNcFileForReadWrite();
		STATUS		checkCodeSyntax();
		void		checkEachLineForSyntax();
		STATUS		checkSyntaxOfPeckDrillCycle(QStringList codelist);
		STATUS		checkSyntaxOfPartingGrooving(QStringList codelist);
		STATUS		checkSyntaxOfRapidCode(QStringList codelist);
		STATUS		checkSyntaxOfLinearInterpol(QStringList codelist);
		STATUS		checkSyntaxOfCWCirInterpol(QStringList codelist);
		STATUS		checkSyntaxOfCCWCirInterpol(QStringList codelist);
		STATUS		checkSyntaxOfDwellCode(QStringList codelist);
		STATUS		checkSyntaxOfCannedTurning(QStringList codelist);
		STATUS		checkSyntaxOfFacingCycle(QStringList codelist);
		STATUS		checkSyntaxOfSinglePassThreading(QStringList codelist);
		STATUS		checkSyntaxOfMultiPassThreading(QStringList codelist);
		QString		getFullNcCodeText();

		friend	class cleanupReadWriteInstance;

	protected:
		NcFanuc10T11TFileReader();									
		NcFanuc10T11TFileReader(const NcFanuc10T11TFileReader &){}									
		NcFanuc10T11TFileReader&	operator=(const NcFanuc10T11TFileReader&);

	private:
		void					buildG74Code();
		void					buildG75Code();
		void					buildG76Code();
		void					buildG00Code();
		void					buildG01Code();
		void					buildG02Code();
		void					buildG03Code();
		void					buildG04Code();
		void					buildG90Code();
		void					buildG92Code();
		void					buildG94Code();
		void					buildG20Code();
		void					buildG21Code();
		void					buildG28Code();
		void					buildMCode(QString code);
		void					buildSCode(QString code);
		void					buildFCode(QString code);
		void					buildIndTCode(QString code);
		void					buildTCode();
		void					handleError(QString code);
		void					handleError(QStringList codelist);
		void					buildIndXCode(QString code);
		void					buildIndZCode(QString code);
		void					resetBoolValues();

	private:
		static NcFanuc10T11TFileReader		*mNcFileReaderInstance;
		QString							mCurrentFileName;
		QFile							*mFile;
		QList<QString>					mCodeBlockList;
		static int						mLineCounter;
		bool							mRCodeDetected;
		bool							mSCodeDetected;
		bool							mFCodeDetected;
		bool							mXValueDetected;
		bool							mZValueDetected;
		bool							mQCodeDetected;
		bool							mTCodeDetected;
		bool							mGCodeDetected;
		bool							mICodeDetected;
		bool							mKCodeDetected;
		bool							mPCodeDetected;
		bool							mDCodeDetected;
		bool							mMachiningCodeDetected;
		bool							mRetractCodeDetected;
		double							Rvalue;
		double							PValue;
		double							QValue;
		double							RetractValue;
		double							FValue;
		double							SValue;
		double							XValue;
		double							ZValue;
		double							IValue;
		double							KValue;
		int								TValue;
		double							DValue;
		int								mDwellTime;
		QTextStream						ncfilereader;
		CODE_Type						mLastExecutedCodeType;
		
						
	};
}



#endif