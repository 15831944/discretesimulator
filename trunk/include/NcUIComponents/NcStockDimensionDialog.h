#ifndef NC_STOCKDIMENSIONDIALOG
#define NC_STOCKDIMENSIONDIALOG

#include <QDialog>
#include "NcUiComponents\NcUIComponentsGlobal.h"

namespace Ui
{
	class Dialog;
}

namespace DiscreteSimulator
{
	class NCUICOMPONENTSEXPORT NcStockDimensionDialog : public QDialog
	{
		Q_OBJECT

	public :
		static			NcStockDimensionDialog* StockDimDialogInstance();
		Ui::Dialog*		mDialog;
		virtual			~NcStockDimensionDialog();
		double			mStockDiameter;
		double			mStockLength;
		int				exec();
		friend class	cleanupStaticWindowInstances;


	public slots:
		void			acceptvalues();
		void			rejectvalues();

	private:
		static NcStockDimensionDialog*	mStockDimDiaInstance;
		
	private:
		NcStockDimensionDialog();
		NcStockDimensionDialog(const NcStockDimensionDialog&);
		NcStockDimensionDialog& operator=(const NcStockDimensionDialog &);
		
	};
}

#endif