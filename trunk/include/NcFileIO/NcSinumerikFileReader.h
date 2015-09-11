#ifndef NC_SINUMERIKFILEREADER
#define NC_SINUMERIKFILEREADER

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
	class NCEXPORT NcSinumerikFileReader : public NcFileReader
	{

	public:
		NcSinumerikFileReader(QString filename);									
						
	protected:
		virtual void		checkEachLineForSyntax();
		void					buildMCode(QString code);				
	};
}


#endif