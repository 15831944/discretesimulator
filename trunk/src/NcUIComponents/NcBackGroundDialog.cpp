#include <QColorDialog>

#include "ui_backgroundDialog.h"
#include "NcUIComponents\NcSimulationWindow.h"
#include "NcUIComponents\NcBackGroundDialog.h"

using namespace DiscreteSimulator;

NcBackGroundDialog*	NcBackGroundDialog::mBackGroundDiaInstance = 0;

NcBackGroundDialog::NcBackGroundDialog() 
{
	mDialog = new Ui::BackgroundDialog();
	mDialog->setupUi(this);
	initValues();
	connect(mDialog->changeColorButton, SIGNAL(clicked()), this, SLOT(changeColor()));
	connect(mDialog->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}

NcBackGroundDialog::~NcBackGroundDialog()
{
	delete mDialog;
}


NcBackGroundDialog* NcBackGroundDialog::backgroundDialogInstance()
{
	if(mBackGroundDiaInstance == 0)
	{
		mBackGroundDiaInstance = new NcBackGroundDialog();
	}
	return mBackGroundDiaInstance;
}


void NcBackGroundDialog::resizeEvent(QResizeEvent *event)
{
	if (mDialog->imageLabelPreview->pixmap() != NULL)
	{
		QSize scaledSize = m_previewPixmap.size();
		scaledSize.scale(mDialog->imageLabelPreview->size(), Qt::KeepAspectRatio);
		if (! mDialog->imageLabelPreview->pixmap() || (scaledSize !=  mDialog->imageLabelPreview->pixmap()->size()))
		{
			updateImageLabelPreview();
		}
	}
	
}

void NcBackGroundDialog::changeColor()
{
    QColor color = QColorDialog::getColor(m_backgroundColor, this);
	if (color.isValid())
	{
        m_backgroundColor = color;
		mDialog->imageLabelPreview->clear();
		mDialog->imageLabelPreview->setPalette(QPalette(m_backgroundColor));			
	}
 }

// Accept modifications
void NcBackGroundDialog::accept()
{
	//Back ground yet to be set
	QDialog::accept();
}


void NcBackGroundDialog::initValues()
{
    mDialog->imageLabelPreview->setPalette(QPalette(m_backgroundColor));
}

// Update the preview pixmap
void NcBackGroundDialog::updateImageLabelPreview()
{
	 mDialog->imageLabelPreview->setPixmap(m_previewPixmap.scaled(mDialog->imageLabelPreview->size(),
													Qt::KeepAspectRatio,
													Qt::SmoothTransformation));	
}


