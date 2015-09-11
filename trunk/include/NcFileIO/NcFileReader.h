#ifndef NCFILEREADER
#define NCFILEREADER

#include <QObject>
#include <QString>
#include <QList>
#include <QTextStream>
#include "NcFileIO\NcFileIOGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"


namespace DiscreteSimulator
{
	class NCEXPORT NcFileReader
	{
	public:
		NcFileReader(QString filename);
		virtual STATUS		checkCodeSyntax();
		virtual QString		getFullNcCodeText();
protected:
		virtual void		checkEachLineForSyntax(){}
		virtual STATUS		checkSyntaxOfRapidCode(QStringList codelist);
		virtual STATUS		checkSyntaxOfLinearInterpol(QStringList codelist);
		virtual STATUS		checkSyntaxOfCWCirInterpol(QStringList codelist);
		virtual STATUS		checkSyntaxOfCCWCirInterpol(QStringList codelist);
		virtual STATUS		checkSyntaxOfDwellCode(QStringList codelist);
		
		virtual void					buildG00Code();
		virtual void					buildG01Code();
		virtual void					buildG02Code();
		virtual void					buildG03Code();
		virtual void					buildG04Code();
		virtual void					buildSCode(QString code);
		virtual void					buildFCode(QString code);
		virtual void					buildIndTCode(QString code);
		virtual void					buildTCode();
		virtual void					handleError(QString code);
		virtual void					handleError(QStringList codelist);
		virtual void					buildG20Code();
		virtual void					buildG21Code();


		QString							mCurrentFileName;
		QList<QString>					mCodeBlockList;
		static int						mLineCounter;
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
		int								mDwellTime;
		QTextStream						ncfilereader;

	
	};


}

#endif