#ifndef NCFANUC0TREADER
#define NCFANUC0TREADER

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
	class NCEXPORT NcFanuc0TFileReader : public NcFileReader  
	{

	public:
		NcFanuc0TFileReader(QString filename);
					
	private:
		STATUS					checkSecondLineForG74();
		STATUS					checkSecondLineForG75();
		STATUS					checkSecondLineForG76();
		void					buildG74Code();
		void					buildG75Code();
		void					buildG76Code();
		void					buildG90Code();
		void					buildG92Code();
		void					buildG94Code();
		void					buildG28Code();
		void					buildMCode(QString code);
		void		checkEachLineForSyntax();
		STATUS		checkSyntaxOfPeckDrillCycle(QStringList codelist);
		STATUS		checkSyntaxOfPartingGrooving(QStringList codelist);
		STATUS		checkSyntaxOfCannedTurning(QStringList codelist);
		STATUS		checkSyntaxOfFacingCycle(QStringList codelist);
		STATUS		checkSyntaxOfSinglePassThreading(QStringList codelist);
		STATUS		checkSyntaxOfMultiPassThreading(QStringList codelist);
	private:
		
		bool							mFirstG74CodeDetected;
		bool							mFirstG76CodeDetected;  
		bool							mRCodeDetected;
		
		int								mDwellTime;
		QTextStream						ncfilereader;
		bool							mFirstG75CodeDetected;
						
	};
}



#endif