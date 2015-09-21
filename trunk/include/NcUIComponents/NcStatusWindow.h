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
	Q_OBJECT
	public :
		static		NcStatusWindow* StatusWindowInstance();
	
		
		virtual		~NcStatusWindow();
		void		updateToolPosition(double mCurrentToolZPos, double mCurrentToolXPos);
		void		updateFeedRate(int feed);
		void		updateSpindleSpeed(double speed);
		void		updateSpindleStatus(bool status);
		void		updateCycleTime(int hr, int min, int sec);
		void        clear();
		friend class cleanupStaticWindowInstances;

/*****************************Pranit**************************/
		void		 updateNCCoolantStat(bool status);	
		QString      getmaterialName();
	private:
		Ui::Form*	mStatusWidget;
		void		 updateMaterial();
		


	signals:
		void        propertyChanged();
public slots :
		void		updateStockProperties();
	


	private:
		static NcStatusWindow*	mStatusWindowInstance;
		
	private:
		NcStatusWindow();
		NcStatusWindow(const NcStatusWindow&);
		NcStatusWindow& operator=(const NcStatusWindow &);
		
	};
}

#endif