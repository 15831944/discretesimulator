#ifndef NC_STATUSWINDOW
#define NC_STATUSWINDOW

#include <QWidget>

#include "NcUiComponents\NcUIComponentsGlobal.h"

namespace Ui
{
	class Form;
}

namespace DiscreteSimulator
{
	
	class NCUICOMPONENTSEXPORT NcStatusWindow : public QWidget
	{
	public :
		static		NcStatusWindow* StatusWindowInstance();
	
		Ui::Form*	mStatusWidget;
		virtual		~NcStatusWindow();
		void		updateToolPosition(double mCurrentToolZPos, double mCurrentToolXPos, double mCurrentToolYPos);
		void		updateFeedRate(int feed);
		void		updateSpindleSpeed(double speed);
		void		updateSpindleStatus(bool status);
		void		updateCycleTime(int hr, int min, int sec);
		friend class cleanupStaticWindowInstances;
	private:
		static NcStatusWindow*	mStatusWindowInstance;
		
	private:
		NcStatusWindow();
		NcStatusWindow(const NcStatusWindow&);
		NcStatusWindow& operator=(const NcStatusWindow &);
		
	};
}

#endif