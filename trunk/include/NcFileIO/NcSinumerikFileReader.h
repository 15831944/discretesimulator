#ifndef NC_SINUMERIKFILEREADER
#define NC_SINUMERIKFILEREADER

#include <QObject>
#include <QString>
#include <QList>
#include <QTextStream>
#include "NcFileIO\NcFileIOGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"

class QFile;

namespace DiscreteSimulator
{
	class NCEXPORT NcSinumerikFileReader  : public QObject
	{
		Q_OBJECT

	public:
		static NcSinumerikFileReader*	getReaderInstance();
		~NcSinumerikFileReader();

		void		setCurrentNcFile(QString filename) {mCurrentFileName = filename;}
		QFile*		getCurrentNcFile();
		bool		openNcFileForReadWrite();
		STATUS		checkCodeSyntax();
		void		checkEachLineForSyntax();		
		STATUS		checkSyntaxOfRapidCode(QStringList codelist);
		STATUS		checkSyntaxOfLinearInterpol(QStringList codelist);
		STATUS		checkSyntaxOfCWCirInterpol(QStringList codelist);
		STATUS		checkSyntaxOfCCWCirInterpol(QStringList codelist);
		STATUS		checkSyntaxOfDwellCode(QStringList codelist);
		QString		getFullNcCodeText();

		friend	class cleanupReadWriteInstance;

	protected:
		NcSinumerikFileReader();									
		NcSinumerikFileReader(const NcSinumerikFileReader &){}									
		NcSinumerikFileReader&	operator=(const NcSinumerikFileReader&);

	private:
		
		void					handleError(QString code);
		void					handleError(QStringList codelist);
		void					buildG00Code();
		void					buildG01Code();
		void					buildG02Code();
		void					buildG03Code();
		void					buildG04Code();
		void					buildG20Code();
		void					buildG21Code();
		void					buildMCode(QString code);
		void					buildSCode(QString code);
		void					buildFCode(QString code);
		void					buildIndTCode(QString code);
		void					buildTCode();
		

	private:
		static NcSinumerikFileReader		*mReaderInstance;
		QString							mCurrentFileName;
		QFile							*mFile;
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