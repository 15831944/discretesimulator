#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include "NcUi\NCUiGlobal.h"

namespace DiscreteSimulator
{

	class NCMainWindow;

	class NCUIEXPORT SplashScreen : public QSplashScreen
	{
		Q_OBJECT

		public:
			SplashScreen(QWidget *parent=0);
			void setMainWindow(NCMainWindow *widget);
			~SplashScreen();
			
	};
}

#endif // SPLASHSCREEN_H
