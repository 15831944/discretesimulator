#include <QSettings>
#include <QString>
#include <QTextStream>

#include "ui_ToolOffsetsDialog.h"
#include "NcStateMachine\T01.h"
#include "NcStateMachine\T02.h"
#include "NcStateMachine\T03.h"
#include "NcStateMachine\T04.h"
#include "NcStateMachine\T05.h"
#include "NcStateMachine\T06.h"
#include "NcStateMachine\T07.h"
#include "NcStateMachine\T08.h"
#include "NcUIComponents\NcSimulationWindow.h"
#include "NcUIComponents\NcToolOffsetsDialog.h"


using namespace DiscreteSimulator;

static char *T1X	= "t1X";
static char *T1Z	= "t1Z";
static char *T2X	= "t2X";
static char *T2Z	= "t2Z";
static char *T3X	= "t3X";
static char *T3Z	= "t3Z";
static char *T4X	= "t4X";
static char *T4Z	= "t4Z";
static char *T5X	= "t5X";
static char *T5Z	= "t5Z";
static char *T6X	= "t6X";
static char *T6Z	= "t6Z";
static char *T7X	= "t7X";
static char *T7Z	= "t7Z";
static char *T8X	= "t8X";
static char *T8Z	= "t8Z";

NcToolOffsetsDialog*	NcToolOffsetsDialog::mInstance = 0;

NcToolOffsetsDialog::NcToolOffsetsDialog() 
{
	mDialog= new Ui::ToolOffsets();
	mDialog->setupUi(this);
	connect(this, SIGNAL(accepted()), this, SLOT(acceptValues()));
	mT1x = 0.1;
	mT1z = 0.1;
	mT2x = 0.1;
	mT2z = 0.1;
	mT3x = 0.1;
	mT3z = 0.1;
	mT4x = 0.1;
	mT4z = 0.1;
	mT5x = 0.1;
	mT5z = 0.1;
	mT6x = 0.1;
	mT6z = 0.1;
	mT7x = 0.1;
	mT7z = 0.1;
	mT8x = 0.1;
	mT8z = 0.1;
				
}


NcToolOffsetsDialog* NcToolOffsetsDialog::getInstance()
{
	if(mInstance == 0)
	{
		mInstance = new NcToolOffsetsDialog();
	}
	return mInstance;
}


NcToolOffsetsDialog::~NcToolOffsetsDialog()
{
	delete mDialog;
}

int NcToolOffsetsDialog::exec()
{
	QSettings settings;
	QString val;

	mT1x			= settings.value(T1X).toDouble();
	QTextStream(&val) << mT1x;
	mDialog->T1X->setText(val);

	mT1z			= settings.value(T1Z).toDouble();
	val.clear();
	QTextStream(&val) << mT1z;
	mDialog->T1Z->setText(val);

	mT2x	= settings.value(T2X).toDouble();
	val.clear();
	QTextStream(&val) << mT2x;
	mDialog->T2X->setText(val);

	mT2z	= settings.value(T2Z).toDouble();
	val.clear();
	QTextStream(&val) << mT2z;
	mDialog->T2Z->setText(val);

	mT3x	= settings.value(T3X).toDouble();
	val.clear();
	QTextStream(&val) << mT3x;
	mDialog->T3X->setText(val);

	mT3z	= settings.value(T3Z).toDouble();
	val.clear();
	QTextStream(&val) << mT3z;
	mDialog->T3Z->setText(val);

	mT4x			= settings.value(T4X).toDouble();
	val.clear();
	QTextStream(&val) << mT4x;
	mDialog->T4X->setText(val);

	mT4z			= settings.value(T4Z).toDouble();
	val.clear();
	QTextStream(&val) << mT4z;
	mDialog->T4Z->setText(val);

	mT5x			= settings.value(T5X).toDouble();
	val.clear();
	QTextStream(&val) << mT5x;
	mDialog->T5X->setText(val);

	mT5z			= settings.value(T5Z).toDouble();
	val.clear();
	QTextStream(&val) << mT5z;
	mDialog->T5Z->setText(val);

	mT6x		= settings.value(T6X).toDouble();
	val.clear();
	QTextStream(&val) << mT6x;
	mDialog->T6X->setText(val);

	mT6z		= settings.value(T6Z).toDouble();
	val.clear();
	QTextStream(&val) << mT6z;
	mDialog->T6Z->setText(val);

	mT7x			= settings.value(T7X).toDouble();
	val.clear();
	QTextStream(&val) << mT7x;
	mDialog->T7X->setText(val);

	mT7z			= settings.value(T7Z).toDouble();
	val.clear();
	QTextStream(&val) << mT7z;
	mDialog->T7Z->setText(val);

	mT8x			= settings.value(T8X).toDouble();
	val.clear();
	QTextStream(&val) << mT8x;
	mDialog->T8X->setText(val);

	mT8z			= settings.value(T8Z).toDouble();
	val.clear();
	QTextStream(&val) << mT8z;
	mDialog->T8Z->setText(val);

	return QDialog::exec();
}

void NcToolOffsetsDialog::acceptValues()
{
	QSettings settings;
	settings.setValue(T1X, mDialog->T1X->text().toDouble());
	settings.setValue(T1Z, mDialog->T1Z->text().toDouble());
	settings.setValue(T2X, mDialog->T2X->text().toDouble());
	settings.setValue(T2Z, mDialog->T2Z->text().toDouble());
	settings.setValue(T3X, mDialog->T3X->text().toDouble());
	settings.setValue(T3Z, mDialog->T3Z->text().toDouble());
	settings.setValue(T4X, mDialog->T4X->text().toDouble());
	settings.setValue(T4Z, mDialog->T4Z->text().toDouble());
	settings.setValue(T5X, mDialog->T5X->text().toDouble());
	settings.setValue(T5Z, mDialog->T5Z->text().toDouble());
	settings.setValue(T6X, mDialog->T6X->text().toDouble());
	settings.setValue(T6Z, mDialog->T6Z->text().toDouble());
	settings.setValue(T7X, mDialog->T7X->text().toDouble());
	settings.setValue(T7Z, mDialog->T7Z->text().toDouble());
	settings.setValue(T8X, mDialog->T8X->text().toDouble());
	settings.setValue(T8Z, mDialog->T8Z->text().toDouble());
	
	T01::setToolXOffset(mDialog->T1X->text().toDouble());
	T01::setToolZOffset(mDialog->T1Z->text().toDouble());

	T02::setToolXOffset(mDialog->T2X->text().toDouble());
	T02::setToolZOffset(mDialog->T2Z->text().toDouble());

	T03::setToolXOffset(mDialog->T3X->text().toDouble());
	T03::setToolZOffset(mDialog->T3Z->text().toDouble());

	T04::setToolXOffset(mDialog->T4X->text().toDouble());
	T04::setToolZOffset(mDialog->T4Z->text().toDouble());

	T05::setToolXOffset(mDialog->T5X->text().toDouble());
	T05::setToolZOffset(mDialog->T5Z->text().toDouble());

	T06::setToolXOffset(mDialog->T6X->text().toDouble());
	T06::setToolZOffset(mDialog->T6Z->text().toDouble());

	T07::setToolXOffset(mDialog->T7X->text().toDouble());
	T07::setToolZOffset(mDialog->T7Z->text().toDouble());

	T08::setToolXOffset(mDialog->T8X->text().toDouble());
	T08::setToolZOffset(mDialog->T8Z->text().toDouble());
	return;
}




