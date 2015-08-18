#include <QTextStream>

#include "ui_statusWindow.h"
#include "NcUIComponents\NcStatusWindow.h"

using namespace DiscreteSimulator;

NcStatusWindow*	NcStatusWindow::mStatusWindowInstance = 0;

NcStatusWindow* NcStatusWindow::StatusWindowInstance()
{
	if(NcStatusWindow::mStatusWindowInstance == 0)
	{
		NcStatusWindow::mStatusWindowInstance = new NcStatusWindow();
	}
	return NcStatusWindow::mStatusWindowInstance;
}


NcStatusWindow::NcStatusWindow()
{
	mStatusWidget = new Ui::Form();
	mStatusWidget->setupUi(this);	
}

NcStatusWindow::~NcStatusWindow()
{
	delete mStatusWidget;
}

void NcStatusWindow::updateToolPosition(double mCurrentToolZPos, double mCurrentToolXPos, double mCurrentToolYPos)
{	
	QString coordval;
	QTextStream(&coordval) << mCurrentToolXPos;

	mStatusWidget->xcoord->setText(coordval);
	coordval.clear();
	QTextStream(&coordval) << mCurrentToolZPos;
	mStatusWidget->zcoord->setText(coordval);
}


void NcStatusWindow::updateFeedRate(int feed)
{
	QString feedval;
	QTextStream(&feedval) << feed;

	mStatusWidget->feed->setText(feedval);
	feedval.clear();
}

void NcStatusWindow::updateSpindleSpeed(double speed)
{
	QString speedval;
	QTextStream(&speedval) << speed;

	mStatusWidget->speed->setText(speedval);
	speedval.clear();
}

void NcStatusWindow::updateSpindleStatus(bool status)
{
	if(status == true)
	{
		mStatusWidget->spindle_ON->setChecked(true);
	}
	else if(status == false)
	{
		mStatusWidget->spindle_OFF->setChecked(true);
	}

}


void	NcStatusWindow::updateCycleTime(int hr, int min, int sec)
{
	QString time;
	QTextStream(&time) << hr;

	mStatusWidget->hour->setText(time);

	time.clear();
	QTextStream(&time) << min;

	mStatusWidget->min->setText(time);

	time.clear();
	QTextStream(&time) << sec;

	mStatusWidget->sec->setText(time);






}
