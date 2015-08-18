#ifndef NC_FILEREADWRITEMANAGER
#define NC_FILEREADWRITEMANAGER

#include <QObject>
#include <QString>
#include "NcFileIO\NcFileIOGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"

class QFile;

namespace DiscreteSimulator
{

	class NCEXPORT NcFileReadWriteManager : public QObject
	{
		Q_OBJECT
				
	public:
		static		NcFileReadWriteManager* getFileReadWriteInstance();
		~NcFileReadWriteManager();

		void		setCurrentNcFile(QString filename) {mCurrentFileName = filename;}
		bool		openNcFileForReadWrite();
		QString		getFullNcCodeText();
		bool		interpreteNcFile();
		void		interpreteCodeBlock(QString codeblock);
		void		intrepreteIndividualCodes(QString code);
		void		interpreteGCode(QString code);
		void		interpreteMCode(QString code);
		void		interpreteSCode(QString code);
		void		interpreteFCode(QString code);
		void		interpreteTCode(QString code);
		void		interpreteXCode(QString code);
		void		interpreteZCode(QString code);
		void		interpreteUCode(QString code);
		void		interpreteWCode(QString code);
		void		interpreteICode(QString code);
		void		interpreteKCode(QString code);
		void		interpreteDCode(QString code);
		STATUS		writeSTLFile(const char *file);
		QFile		*getCurrentNcFile();

		friend	class cleanupReadWriteInstance;

	protected:
		NcFileReadWriteManager();									
		NcFileReadWriteManager(const NcFileReadWriteManager &){}									
		NcFileReadWriteManager&	operator=(const NcFileReadWriteManager&);
	
	private:
		static NcFileReadWriteManager	*mNcFileReadWriteInstance;
		QString							mCurrentFileName;
		QFile							*mFile;

	};
}






#endif