#include <QDoubleValidator>
#include <QSettings>
#include <QString>
#include <QTextStream>

#include "ui_stockDimensionDialog.h"
#include "NcUIComponents\NcStockDimensionDialog.h"


using namespace DiscreteSimulator;

static char *StockDiameter			= "stockDiameter";
static char *StockLength			= "stockLength";


NcStockDimensionDialog*	NcStockDimensionDialog::mStockDimDiaInstance = 0;


NcStockDimensionDialog::NcStockDimensionDialog()
{
	mDialog = new Ui::Dialog();
	mStockDiameter = 30;
	mStockLength = 80;
	mDialog->setupUi(this);

	QDoubleValidator *inputvalidator = new QDoubleValidator(0, 2000, 4, this);
	mDialog->stockdia->setValidator(inputvalidator);
	mDialog->stockLength->setValidator(inputvalidator);
	inputvalidator->setNotation(QDoubleValidator::StandardNotation);

	connect(mDialog->buttonBox, SIGNAL(accepted()), this, SLOT(acceptvalues()));
	connect(mDialog->buttonBox, SIGNAL(rejected()), this, SLOT(rejectvalues()));
}


NcStockDimensionDialog* NcStockDimensionDialog::StockDimDialogInstance()
{
	if(NcStockDimensionDialog::mStockDimDiaInstance == 0)
	{
		NcStockDimensionDialog::mStockDimDiaInstance = new NcStockDimensionDialog();
	}
	return NcStockDimensionDialog::mStockDimDiaInstance;
}



NcStockDimensionDialog::~NcStockDimensionDialog()
{
	delete mDialog;
}


int NcStockDimensionDialog::exec()
{
	QSettings settings;
	QString val;

	mStockDiameter		= settings.value(StockDiameter).toDouble();
	val.clear();
	QTextStream(&val) << mStockDiameter;
	mDialog->stockdia->setText(val);

	mStockLength		= settings.value(StockLength).toDouble();
	val.clear();
	QTextStream(&val) << mStockLength;
	mDialog->stockLength->setText(val);

	return QDialog::exec();
}



void	NcStockDimensionDialog::acceptvalues()
{

	if(!mDialog->stockdia->text().isEmpty())
	{
		mStockDiameter = mDialog->stockdia->text().toDouble();
	}
	
	if(!mDialog->stockLength->text().isEmpty())
	{
		mStockLength = mDialog->stockLength->text().toDouble();
	}

	QSettings settings;
	settings.setValue(StockDiameter, mDialog->stockdia->text().toDouble());
	settings.setValue(StockLength, mDialog->stockLength->text().toDouble());
}

void	NcStockDimensionDialog::rejectvalues()
{}

