#ifndef NC_JOGMODEDIALOG
#define NC_JOGMODEDIALOG

#include <QDialog>

#include "NcUiComponents\NcUIComponentsGlobal.h"

namespace Ui {
    class jogModeDialog;
} 

namespace DiscreteSimulator
{
	class NCUICOMPONENTSEXPORT NcJogModeDialog : public QDialog
	{
		Q_OBJECT

	public :
		static					NcJogModeDialog* jogModeDialogInstance();
		Ui::jogModeDialog*		mDialog;
		virtual					~NcJogModeDialog();
		
		friend class	cleanupStaticWindowInstances;

	private:
		static NcJogModeDialog*	mJogModeDiaInstance;
		
	private:
		NcJogModeDialog();
		NcJogModeDialog(const NcJogModeDialog&);
		NcJogModeDialog& operator=(const NcJogModeDialog &);		
	};
}

#endif