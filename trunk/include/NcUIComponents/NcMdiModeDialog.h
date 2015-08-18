#ifndef NC_MDIMODEDIALOG
#define NC_MDIMODEDIALOG

#include <QDialog>

#include "NcUiComponents\NcUIComponentsGlobal.h"

#include <QRegExpValidator>

namespace Ui
{
	class mdiModeDialog;
}


namespace DiscreteSimulator
{
	class NCUICOMPONENTSEXPORT NcMdiModeDialog : public QDialog
	{
		Q_OBJECT

	public :
		static					NcMdiModeDialog* mdiModeDialogInstance();
		Ui::mdiModeDialog*		mDialog;
		virtual					~NcMdiModeDialog();
		
		friend class	cleanupStaticWindowInstances;

	public slots:
		void				validateInputAndExecute();
		void				checkEnteredText(const QString &text);

	private:
		static NcMdiModeDialog*	mMdiModeDiaInstance;
		
	private:
		NcMdiModeDialog();
		NcMdiModeDialog(const NcMdiModeDialog&);
		NcMdiModeDialog& operator=(const NcMdiModeDialog &);

		
		QRegExpValidator *GCodeValidator; 	
 		QRegExpValidator *NCodeValidator;
		QRegExpValidator *MCodeValidator;
		QRegExpValidator *FCodeValidator;
		QRegExpValidator *XCodeValidator;
		QRegExpValidator *ZCodeValidator;		
		QRegExpValidator *UCodeValidator;	    
		QRegExpValidator *WCodeValidator;	 	
		QRegExpValidator *SCodeValidator;	 	
		QRegExpValidator *TCodeValidator;	  
		QRegExpValidator *ICodeValidator;	   
		QRegExpValidator *KCodeValidator;
		QRegExpValidator *generalCodeValidator;
	
		
	};
}

#endif