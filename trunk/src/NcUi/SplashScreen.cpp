#include "NcUi\SplashScreen.h"
#include "NcUi\NCMainWindow.h"

using namespace DiscreteSimulator;

SplashScreen::SplashScreen(QWidget *parent) : 
QSplashScreen(parent)
{
	setPixmap(QPixmap(tr(":/tools/images/splash1.png")));
}

SplashScreen::~SplashScreen(){}

