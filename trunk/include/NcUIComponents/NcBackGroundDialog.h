#ifndef NC_BACKGROUNDDIALOG
#define NC_BACKGROUNDDIALOG

#include <QDialog>
#include <QResizeEvent>

#include "NcUiComponents\NcUIComponentsGlobal.h"
#include <QColor>

namespace Ui {
	class BackgroundDialog;
}


namespace DiscreteSimulator
{
	class NCUICOMPONENTSEXPORT NcBackGroundDialog : public QDialog
	{
		Q_OBJECT
		
	public :
		static					NcBackGroundDialog* backgroundDialogInstance();
		Ui::BackgroundDialog*	mDialog;
		virtual					~NcBackGroundDialog();
		void					resizeEvent(QResizeEvent *event);
		void					updateImageLabelPreview();
		friend class	cleanupStaticWindowInstances;
		
	private:
		static NcBackGroundDialog*	mBackGroundDiaInstance;
		QColor						m_backgroundColor;
		QPixmap						m_previewPixmap;
		
	private:
		NcBackGroundDialog();
		NcBackGroundDialog(const NcBackGroundDialog&);
		NcBackGroundDialog& operator=(const NcBackGroundDialog &);		

		void		initValues();

	public slots:
		void		changeColor();
		void		accept();
	};
}

#endif