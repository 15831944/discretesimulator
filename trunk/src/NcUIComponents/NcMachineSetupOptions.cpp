#include <QColorDialog>
#include <QSettings>
#include <QString>
#include <QTextStream>

#include "ui_MachineSetupOptions.h"
#include "NcUIComponents\NcSimulationWindow.h"
#include "NcUIComponents\NcMachineSetupOptions.h"



using namespace DiscreteSimulator;

static char *BacklashX			= "backlashX";
static char *BacklashZ			= "backlashZ";
static char *BallScrewPitchX	= "ballScrewPitchX";
static char *BallScrewPitchZ	= "ballScrewPitchZ";
static char *AxisDriveRatioX	= "axisDriveRatioX";
static char *AxisDriveRatioZ	= "axisDriveRatioZ";
static char *MinSpeedX			= "minSpeedX";
static char *MinSpeedZ			= "minSpeedZ";
static char *MaxSpeedX			= "maxSpeedX";
static char *MaxSpeedZ			= "maxSpeedZ";
static char *AccelerationX		= "AccelerationX";
static char *AccelerationZ		= "AccelerationZ";

NcMachineSetupOptions*	NcMachineSetupOptions::mMacSetupOptionsInstance = 0;

NcMachineSetupOptions::NcMachineSetupOptions() 
{
	mDialog = new Ui::MacSetupOptions();
	mDialog->setupUi(this);
	connect(this, SIGNAL(accepted()), this, SLOT(acceptValues()));
	mBacklashX = 0.1;
	mBacklashZ = 0.1;
	mBallScrewPitchX = 0.1;
	mBallScrewPitchZ = 0.1;
	mAxisDriveRatioX = 0.1;
	mAxisDriveRatioZ = 0.1;
	mMinSpeedX = 0.1;
	mMinSpeedZ = 0.1;
	mMaxSpeedX = 0.1;
	mMaxSpeedZ = 0.1;
	mAccelerationX = 0.1;
	mAccelerationZ = 0.1;
				
}

NcMachineSetupOptions::~NcMachineSetupOptions()
{
	delete mDialog;
}

int NcMachineSetupOptions::exec()
{
	QSettings settings;
	QString val;
	
	mBacklashX			= settings.value(BacklashX).toDouble();
	QTextStream(&val) << mBacklashX;
	mDialog->backlashX->setText(val);

	mBacklashZ			= settings.value(BacklashZ).toDouble();
	val.clear();
	QTextStream(&val) << mBacklashZ;
	mDialog->backlashZ->setText(val);

	mBallScrewPitchX	= settings.value(BallScrewPitchX).toDouble();
	val.clear();
	QTextStream(&val) << mBallScrewPitchX;
	mDialog->BallScrewPitchX->setText(val);

	mBallScrewPitchZ	= settings.value(BallScrewPitchZ).toDouble();
	val.clear();
	QTextStream(&val) << mBallScrewPitchZ;
	mDialog->BallScrewPitchZ->setText(val);

	mAxisDriveRatioX	= settings.value(AxisDriveRatioX).toDouble();
	val.clear();
	QTextStream(&val) << mAxisDriveRatioX;
	mDialog->AxisDRX->setText(val);

	mAxisDriveRatioZ	= settings.value(AxisDriveRatioZ).toDouble();
	val.clear();
	QTextStream(&val) << mAxisDriveRatioZ;
	mDialog->AxisDRZ->setText(val);

	mMinSpeedX			= settings.value(MinSpeedX).toDouble();
	val.clear();
	QTextStream(&val) << mMinSpeedX;
	mDialog->MinSpeedX->setText(val);

	mMinSpeedZ			= settings.value(MinSpeedZ).toDouble();
	val.clear();
	QTextStream(&val) << mMinSpeedZ;
	mDialog->MinSpeedZ->setText(val);

	mMaxSpeedX			= settings.value(MaxSpeedX).toDouble();
	val.clear();
	QTextStream(&val) << mMaxSpeedX;
	mDialog->MaxSpeedX->setText(val);

	mMaxSpeedZ			= settings.value(MaxSpeedZ).toDouble();
	val.clear();
	QTextStream(&val) << mMaxSpeedZ;
	mDialog->MaxSpeedZ->setText(val);

	mAccelerationX		= settings.value(AccelerationX).toDouble();
	val.clear();
	QTextStream(&val) << mAccelerationX;
	mDialog->AccelerationX->setText(val);

	mAccelerationZ		= settings.value(AccelerationZ).toDouble();
	val.clear();
	QTextStream(&val) << mAccelerationZ;
	mDialog->AccelerationZ->setText(val);

	return QDialog::exec();
}

void NcMachineSetupOptions::acceptValues()
{
	QSettings settings;
	settings.setValue(BacklashX, mDialog->backlashX->text().toDouble());
	settings.setValue(BacklashZ, mDialog->backlashZ->text().toDouble());

	settings.setValue(BallScrewPitchX, mDialog->BallScrewPitchX->text().toDouble());
	settings.setValue(BallScrewPitchZ, mDialog->BallScrewPitchZ->text().toDouble());
	settings.setValue(AxisDriveRatioX, mDialog->AxisDRX->text().toDouble());
	settings.setValue(AxisDriveRatioZ, mDialog->AxisDRZ->text().toDouble());
	settings.setValue(MinSpeedX, mDialog->MinSpeedX->text().toDouble());
	settings.setValue(MinSpeedZ, mDialog->MinSpeedZ->text().toDouble());
	settings.setValue(MaxSpeedX, mDialog->MaxSpeedX->text().toDouble());
	settings.setValue(MaxSpeedZ, mDialog->MaxSpeedZ->text().toDouble());
	settings.setValue(AccelerationX, mDialog->AccelerationX->text().toDouble());
	settings.setValue(AccelerationZ, mDialog->AccelerationZ->text().toDouble());


	return;
}


NcMachineSetupOptions* NcMachineSetupOptions::getInstance()
{
	if(mMacSetupOptionsInstance == 0)
	{
		mMacSetupOptionsInstance = new NcMachineSetupOptions();
	}
	return mMacSetupOptionsInstance;
}

