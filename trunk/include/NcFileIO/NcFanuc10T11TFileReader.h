#ifndef NCFANUC10T11TFILEREADER
#define NCFANUC10T11TFILEREADER

#include <QObject>
#include <QString>
#include <QList>
#include <QTextStream>
#include "NcFileIO\NcFileIOGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
#include "NcFileIO\NcFileReader.h"

class QFile;

namespace DiscreteSimulator
{
	class NCEXPORT NcFanuc10T11TFileReader: public NcFileReader
	{
	
	public:
		NcFanuc10T11TFileReader(QString filename);									
		

	private:
		void					buildG74Code();
		void					buildG75Code();
		void					buildG76Code();
		
		void					buildG90Code();
		void					buildG71Code();
		void					buildG91Code();	
		

		void					buildG92Code();
		void					buildG94Code();
	
		void					buildG28Code();
		void					buildMCode(QString code);
		void					buildIndXCode(QString code);
		void					buildIndZCode(QString code);
		
		void					checkEachLineForSyntax();
		STATUS		checkSyntaxOfPeckDrillCycle(QStringList codelist);
		STATUS		checkSyntaxOfPartingGrooving(QStringList codelist);
		STATUS		checkSyntaxOfCannedTurning(QStringList codelist);
		STATUS		checkSyntaxOfFacingCycle(QStringList codelist);
		STATUS		checkSyntaxOfSinglePassThreading(QStringList codelist);
		STATUS		checkSyntaxOfMultiPassThreading(QStringList codelist);
		STATUS		buildG53Code(QStringList codelist);
		STATUS		buildG54Code(QStringList codelist);
		void		compileExternalNcfile(QString codelist);
		STATUS		checkSyntaxOfIncrementalMove( QStringList codelist );
		STATUS		checkSyntaxOfRoughTurningCycle( QStringList codelist );
	private:
		bool							mRCodeDetected;
		
		bool							mUCodeDetected;
		bool							mWCodeDetected;
		bool							mNCodeDetected;
		bool							mDCodeDetected;
		
		double							Uvalue;
		double							WValue;
		double							NValue;
		int								linecheck;    //for G71 code

		double							DValue;
		int								mDwellTime;
		CODE_Type						mLastExecutedCodeType;
		double							xtemp; //added for compensate the x offset in incremental programming mode G91
		double							ztemp; //added for compensate the z offset in incremental programming mode G91
						
	};
}



#endif