#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QMessageBox>

#include "ui_mdiModeDialog.h"
#include "NcUIComponents\NcMdiModeDialog.h"

using namespace DiscreteSimulator;

NcMdiModeDialog*	NcMdiModeDialog::mMdiModeDiaInstance = 0;

NcMdiModeDialog::NcMdiModeDialog()
{
	mDialog = new Ui::mdiModeDialog();
	mDialog->setupUi(this);
	mDialog->textEdit->setReadOnly(true);
	connect(mDialog->lineEdit, SIGNAL( returnPressed() ), this, SLOT( validateInputAndExecute() ));
	connect(mDialog->lineEdit, SIGNAL( textChanged(const QString &)), this, SLOT(checkEnteredText(const QString &)));

	QRegExp Gpattern( "\\b[G][0-9]{1,2}\\b" );
	GCodeValidator = new QRegExpValidator( (Gpattern), 0);
	
 	QRegExp Npattern( "\\b[N][0-9]{1,4}\\b" );
 	NCodeValidator = new QRegExpValidator( (Npattern), 0);
	
    QRegExp Mpattern( "\\b[M][0-9]{1,3}\\b" );
	MCodeValidator = new QRegExpValidator( (Mpattern), 0);
	
    QRegExp Fpattern( "\\b[F][0-9]{1,5}\\b" );
    FCodeValidator = new QRegExpValidator( (Fpattern), 0);
	
	QRegExp Xpattern( "\\b[X][+-]?\\d*\\.?\\d*\\b" );
	XCodeValidator = new QRegExpValidator( (Xpattern), 0);
	
    QRegExp Zpattern( "\\b[Z][+-]?\\d*\\.?\\d*\\b" );
    ZCodeValidator = new QRegExpValidator( (Zpattern), 0);
	
	QRegExp Upattern( "\\b[U][+-]?\\d*\\.?\\d*\\b" );
    UCodeValidator = new QRegExpValidator( (Upattern), 0);
	
    QRegExp Wpattern( "\\b[W][+-]?\\d*\\.?\\d*\\b" );
	WCodeValidator = new QRegExpValidator( (Wpattern), 0);
	
 	QRegExp Spattern( "\\b[S][0-9]{1,5}\\b" );
	SCodeValidator = new QRegExpValidator( (Spattern), 0);
	
 	QRegExp Tpattern( "\\b[T][0-9]{1,4}\\b" );
	TCodeValidator = new QRegExpValidator( (Tpattern), 0);
	
    QRegExp Ipattern( "\\b[I][+-]?\\d*\\.?\\d*\\b" );
	ICodeValidator = new QRegExpValidator( (Ipattern), 0);
	
    QRegExp Kpattern( "\\b[K][+-]?\\d*\\.?\\d*\\b" );
	KCodeValidator = new QRegExpValidator( (Kpattern), 0);
	
	QRegExp generalPattern( "\\b[XZSTUWFGNMIK][+-]?\\d*\\.?\\d*\\b" );
	generalCodeValidator = new QRegExpValidator( (generalPattern), 0);

}

NcMdiModeDialog::~NcMdiModeDialog()
{
	delete mDialog;
}


NcMdiModeDialog* NcMdiModeDialog::mdiModeDialogInstance()
{
	if(NcMdiModeDialog::mMdiModeDiaInstance == 0)
	{
		NcMdiModeDialog::mMdiModeDiaInstance = new NcMdiModeDialog();
	}
	return NcMdiModeDialog::mMdiModeDiaInstance;
}

void NcMdiModeDialog::checkEnteredText(const QString &text)
{
	
}

void NcMdiModeDialog::validateInputAndExecute() 
{
	QString command(mDialog->lineEdit->text());

	QStringList codes = command.split(QRegExp("\\s+"));

	int pos;
	foreach(QString code, codes)
	{
		switch(code.at(0).toAscii())
		{
		case 'G':
			{
				if(GCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					int n = code.right(code.size() - 1).toInt();
					if(n != 0 && n != 1 && n != 2 && n != 3 && n != 90 && n != 86 && n != 74)
					{
						QString message = tr("The code: ") + code + tr(" is invalid.");
						QMessageBox::warning(0, tr("MDI Mode"), message);	
					}
					//execute code as per the priority 
				}
				break;
			}
		case 'M':
			{
				if(MCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					int n = code.right(code.size() - 1).toInt();
					if(n != 1 && n != 2 && n != 3 && n != 4 && n != 5 && n != 7 && n != 8 && n != 9 && n != 30)
					{
						QString message = tr("The code: ") + code + tr(" is invalid.");
						QMessageBox::warning(0, tr("MDI Mode"), message);	
					}
					//execute code as per the priority 
				}
				break;
			}
		case 'T':
			{
				if(TCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					int n = code.right(code.size() - 1).toInt();
					if(n != 1 && n != 2 && n != 3 && n != 4 && n != 5 && n != 7 && n != 8 && n != 6)
					{
						QString message = tr("The code: ") + code + tr(" is invalid.");
						QMessageBox::warning(0, tr("MDI Mode"), message);	
					}
					//execute code as per the priority 
				}
				break;
			}
		case 'N':
			{
				if(NCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		case 'X':
			{
				if(XCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		case 'Z':
			{
				if(ZCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		case 'U':
			{
				if(UCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		case 'W':
			{
				if(WCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		case 'F':
			{
				if(FCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		case 'S':
			{
				if(SCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		case 'I':
			{
				if(ICodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		case 'K':
			{
				if(KCodeValidator->validate(code, pos) == QValidator::Acceptable)
				{
					//execute code as per the priority 
				}
				break;
			}
		default:
			{
				QString message = tr("The code: ") + code + tr(" is invalid.");
				QMessageBox::warning(0, tr("MDI Mode"), message);
			}
		}
	}

	//may be required to be moved so that command is entered in history after being executed completely
	mDialog->textEdit->setText(mDialog->textEdit->toPlainText() + tr("\n") + mDialog->lineEdit->text());
}










	

	
	