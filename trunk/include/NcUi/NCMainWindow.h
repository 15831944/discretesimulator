#ifndef NC_MAINWINDOW
#define NC_MAINWINDOW

#include <QMainWindow>
#include "NcUi\NCUiGlobal.h"


class 	QTextEdit;
class	QString;
class 	QProgressBar;
class	QFile;
class	QTimer;
class	QSlider;
class	QWidget;
class	QPushButton;
namespace Ui{
	class MainWindow;
}

class NcToolPathWindow;
class NcSimulationWindow;

namespace DiscreteSimulator
{
	
	class NCUIEXPORT NCMainWindow : public QMainWindow
	{
		Q_OBJECT

	public:
		NCMainWindow(QWidget *parent = 0);
		~NCMainWindow();

	public slots:
		void					prepareWindowForNewFile();
		void					open();
		bool					save();
		bool 					saveAs();
		void					closeFile();
		void 					openFile(QString fname = "");
		void 					toggleGrpahicsWindow();
		void 					toggleNcCodeWindow();		
		void 					toggleToolPathWindow();	
		void 					togglePropertyWindow();	
		void 					toggleToolWindow();
		void 					documentWasModified();
		void					run();
		void					pause();
		void					next();
		void					updateWidgets();
		void					changeSimulationSpeed(int interval);
		void					openRecentFile();
		void					setStockDimensions();
		void					executeMDIMode();
		void					executeJOGMode();
		void					updateCodeEditor(int);
		void					updateToolPos(double mCurrentToolZPos, double mCurrentToolXPos);
		void					updateFeedRate(int feed);
		void					updateSpeed(double);
		void					updateSpindleStatus(bool status);
		
		/****************Pranit*************************/
		void					updateCoolantState(bool status);
		void					buildEditorText();
		void					userDefinedStockValues(double stockDiameter, double stockLength);
		void					updateStockProperties();
		void					updateBgcolor(QColor bgcolor);
		void					OpenBackgroundColorDialog();
		void					updateToolPDis(int index);

		void					setSimulationModeToByBlock();
		void					setSimulationModeToByCycle();
		bool					export_STL();
		void					enableRewind();
		void					rewindSimulation();
		void					updateCycleTimeInStatusWin(int hr, int min, int sec);
		void					updateSimWinForBlockMode();
		void					OpenMacSetupOptionsDialog();
		void					OpenToolOffsetsDialog();

	protected:
		void					closeEvent(QCloseEvent * ce);
		void					init();
	private:
		void					setupNcCodeEditor();
		void					createConnections();
		bool					isSaved();
	
		bool 					saveFile(const QString &fileName);
		void 					setCurrentFile(const QString &fileName);
		QString 				strippedName(const QString &fullFileName);
		void					setupGLWidgetWindows();
		void					setupSimulationSpeedController();
		void					setupVideoCaptureWindow();
		void					createRecentFileActions();
		void					updateRecentFileActions();
		void					readSettings();
		void					writeSettings();
		void					cleanUpData();
	private:
		Ui::MainWindow*			mMainWindowUI;
		QString					curFileName;
		QProgressBar            *mpProgressBar;
		QTimer					*mUpdateTimer;
		QSlider					*mSpeedControllerSlider;

		NcToolPathWindow*		m_toolPathWindow;
		NcSimulationWindow*		m_simulationWindow;

		static const int		MaxRecentFiles = 5;
   		QAction					*recentFileActions[MaxRecentFiles];
		QAction					*separatorAct;
		bool					mSimulationRunning;
		bool					mIsNewFile;
		QWidget*				mCodeWindowBaseWidget;
		QPushButton*			mPbCodeWindowOK;
		QPushButton*			mPbCodeWindowCancel;
	protected:
		void				keyPressEvent(QKeyEvent *event ); 
		
	};
}

#endif