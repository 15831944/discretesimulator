#ifndef NC_TOOLCONTROLLER
#define NC_TOOLCONTROLLER

#include "NcStateMachine\NcStateMachineGlobal.h"
#include "NcUtility\NcGlobalDefinitions.h"
#include <QObject>


namespace DiscreteSimulator
{
	class NcStlImport;

	class NCSTATEMACHINEEXPORT NcToolController : public QObject
	{
		Q_OBJECT

	public:
		/*~NcToolController();*/
		static NcToolController*	getToolControllerInstance();
	
		void						display_toolholder(double X, double Y, double Z);
		void						setCurrentToolType(CODE_Type tooltype) {mCurrentToolType = tooltype;}
		void						updateToolPosition(double x, double y, double z);
		void						changeTool(CODE_Type tooltype);
		void						displayToolAtDwellPos();

		

	public:
		double						mCurrentToolXPos;
		double						mCurrentToolYPos;
		double						mCurrentToolZPos;

		double						mToolStartXPos;
		double						mToolStartYPos;
		double						mToolStartZPos;

	public slots:
		void						updateFeedRate(int feed);
		void						updateSpeed(double speed);
		void						updateNcSpindleStatus(bool status);
		void						updateTPDisplay(int index);
		void						updateCycleTime(int, int, int);

	signals:
		void						updateToolPosInStatusWindow(double mCurrentToolZPos, double mCurrentToolXPos, double mCurrentToolYPos);
		void						updateFeedRateInStatusWindow(int);
		void						updateSpeedInStatusWindow(double);
		void						updateSpindleStat(bool);
		void						updateToolPathDisplayInTPWin(int);
		void						updateCycleTimeStatus(int, int, int);

	
	private:
		NcToolController();
		NcToolController(const NcToolController&);
		NcToolController& operator=(const NcToolController&);

		void						material_tool();


	private:
		static NcToolController		*mToolControllerInstance;
		NcStlImport					*stlToolImporter;
		CODE_Type					mCurrentToolType;
		bool						mToolsLoaded;
	};
}


#endif