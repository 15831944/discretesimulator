#ifndef NC_TOOLOFFSETDIALOG
#define NC_TOOLOFFSETDIALOG

#include <QDialog>

#include "NcUiComponents\NcUIComponentsGlobal.h"
namespace Ui
{
	class ToolOffsets;
}
namespace DiscreteSimulator
{
	class NCUICOMPONENTSEXPORT NcToolOffsetsDialog : public QDialog
	{
		Q_OBJECT
		
	public :
		static					NcToolOffsetsDialog* getInstance();
		Ui::ToolOffsets*			mDialog;
		virtual					~NcToolOffsetsDialog();
		int						exec();
		friend class			cleanupStaticWindowInstances;

	public slots:
		void					acceptValues();
		
	private:
		static NcToolOffsetsDialog*	mInstance;

		double					mT1x;
		double					mT1z;
		double					mT2x;
		double					mT2z;
		double					mT3x;
		double					mT3z;
		double					mT4x;
		double					mT4z;
		double					mT5x;
		double					mT5z;
		double					mT6x;
		double					mT6z;
		double					mT7x;
		double					mT7z;
		double					mT8x;
		double					mT8z;

	private:
		NcToolOffsetsDialog();
		NcToolOffsetsDialog(const NcToolOffsetsDialog&);
		NcToolOffsetsDialog& operator=(const NcToolOffsetsDialog&);		
	};
}

#endif