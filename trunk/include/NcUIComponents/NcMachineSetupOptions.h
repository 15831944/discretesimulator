#ifndef NC_MACHINESETUPOPTIONSDIALOG
#define NC_MACHINESETUPOPTIONSDIALOG

#include <QDialog>

#include "NcUiComponents\NcUIComponentsGlobal.h"
namespace Ui
{
	class MacSetupOptions;
}
namespace DiscreteSimulator
{
	class NCUICOMPONENTSEXPORT NcMachineSetupOptions : public QDialog
	{
		Q_OBJECT
		
	public :
		static					NcMachineSetupOptions* getInstance();
		Ui::MacSetupOptions*		mDialog;
		virtual					~NcMachineSetupOptions();
		int						exec();
		friend class			cleanupStaticWindowInstances;

	public slots:
		void					acceptValues();
		
	private:
		static NcMachineSetupOptions*	mMacSetupOptionsInstance;
		double					mBacklashX;
		double					mBacklashZ;
		double					mBallScrewPitchX;
		double					mBallScrewPitchZ;
		double					mAxisDriveRatioX;
		double					mAxisDriveRatioZ;
		double					mMinSpeedX;
		double					mMinSpeedZ;
		double					mMaxSpeedX;
		double					mMaxSpeedZ;
		double					mAccelerationX;
		double					mAccelerationZ;
				
	private:
		NcMachineSetupOptions();
		NcMachineSetupOptions(const NcMachineSetupOptions&);
		NcMachineSetupOptions& operator=(const NcMachineSetupOptions &);		
	};
}

#endif