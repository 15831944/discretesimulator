#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>

#include "ui_jogModeDialog.h"
#include "NcUIComponents\NcJogModeDialog.h"

using namespace DiscreteSimulator;

NcJogModeDialog*	NcJogModeDialog::mJogModeDiaInstance = 0;

NcJogModeDialog::NcJogModeDialog()
{
	mDialog =  new Ui::jogModeDialog();
	mDialog->setupUi(this);
}

NcJogModeDialog::~NcJogModeDialog()
{
	delete mDialog;
}


NcJogModeDialog* NcJogModeDialog::jogModeDialogInstance()
{
	if(NcJogModeDialog::mJogModeDiaInstance == 0)
	{
		NcJogModeDialog::mJogModeDiaInstance = new NcJogModeDialog();
	}
	return NcJogModeDialog::mJogModeDiaInstance;
}

