#include	<QApplication>
#include	<QSplashScreen>
#include	<QTimer>
#include	<QDebug>
#include	<QtGui>
#include	<QGLFormat>
#include	"NcUi\NCMainWindow.h"
#include	"NcUi\SplashScreen.h"

using namespace DiscreteSimulator;

int main(int argc, char *argv[])
{


		QApplication app(argc, argv);

		app.setOrganizationName(QObject::tr("CCTech"));
		app.setApplicationName(QObject::tr("DiscreteSimulator"));
		app.setApplicationVersion(QObject::tr("1.0.0"));

		QSettings::setDefaultFormat(QSettings::IniFormat);
		if (!QGLFormat::hasOpenGL()) {
			 qFatal("This system has no OpenGL support");
			return 1;
		}

		//creation of Splashscreen

	   SplashScreen *splash = new SplashScreen();
	   const int timeToWait = 1000*2; //2 sec
	   splash->show();
	   splash->raise();
	   app.processEvents();
	   splash->showMessage(QObject::tr("Setting up the main window..."),
						Qt::AlignRight | Qt::AlignTop, Qt::white);

	   app.processEvents();
	   NCMainWindow mw;
	   splash->showMessage(QObject::tr("Loading modules..."),
						 Qt::AlignRight | Qt::AlignTop, Qt::white);
	   app.processEvents();
	   splash->showMessage(QObject::tr("www.cctech.co.in"),
						  Qt::AlignLeft | Qt::AlignTop, Qt::green);
	  
	   
	   QTimer::singleShot(timeToWait, splash, SLOT(close()));
	   QTimer::singleShot(timeToWait, &mw, SLOT(show()));
	   
	   //delete splash;
	   return app.exec();
}
