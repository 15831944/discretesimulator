#ifndef NC_GLWIDGET
#define NC_GLWIDGET

#include <QGLWidget>
#include <QPaintEvent>
#include "NcUiComponents\NcUIComponentsGlobal.h"

#define PICK_TOL 5
#define PICK_BUFFER_SIZE 256

class QMouseEvent;
class QWheelEvent;

class QWidget;

namespace DiscreteSimulator
{

	class NCUICOMPONENTSEXPORT NcSimulationWindow : public QGLWidget
	{
		Q_OBJECT
		class ViewHandle
		{
		public:
			ViewHandle();
			~ViewHandle();

			void	zoom				(double inZoom){mViewOrtho+=inZoom*mViewOrtho;}
			void	spin				(int inSpin){mSpin+=inSpin;}
			void	resetAspectRatio	(int width, int height);
			void	resetOrtho			(double	inViewOrtho);
			void	setView				();
			void	resetView			();
			void	resetAll			();
			void	rotate				(double inX,double inY);
			void	pan					(int fromX,int fromY,int toX,int toY);
	/***Pranit***/
			void	updateBackgroundDL(QColor bgcolor);
		
		private:
			void	drawBackground		();
			void	setViewVolume		();
			void	genBackgroundDL		();
			void	resetLight			();

		private:
			double		mAspectRatio;
			double		mViewOrtho;
			double		mDPan[3];
			double		mAngle[2];
			int			mSpin;
			int			mObjectVPSize[2];
		
			GLuint		mBackgroundDL;
			
			GLdouble	mMVmatrix[16];
			GLdouble	mPVmatrix[16];
		};
	public:
		
		NcSimulationWindow(QWidget *parent = 0);
		bool				updateSimulationWindow();
		void				pause();
		void				setBoolForSimControl(bool val) {mBoolForSimControl = val;}
	/*	virtual				~NcSimulationWindow();*/
		void				reinitializeSimControllers();
	
		void				setIsNewFile( bool isnewfile);
		void				callUpdateBg(QColor colorId);

     public slots:
		void	zoomIn(){mViewHandle.zoom(-0.1);updateGL();}
		void	zoomOut(){mViewHandle.zoom(0.1);updateGL();}
		void	zoomToWindow(){mViewHandle.zoom(1);updateGL();}
		void	rotate(QPoint);
		void	spinLeft(){mViewHandle.spin(-10);}
		void	spinRight(){mViewHandle.spin(10);}
		void	pan(QPoint);
		void	frontView();
		void	backView();
		void	topView();
		void	bottomView();
		void	lhSideView();
		void	rhSideView();
		void	isometricView();


	public slots:
	     
		void				setFrontView();
		void				setTopView();
		void				setSideView();
		void				setIsoView();
		void				zoomin();
		void				zoomout();

	signals:
		void				xRotationChanged(double angle);
		void				yRotationChanged(double angle);
		void				zRotationChanged(double angle);

	protected:
		void				initializeGL();
		void				paintGL();
		void				resizeGL(int width,int height);
		void				mousePressEvent(QMouseEvent *event);
		void				mouseMoveEvent(QMouseEvent *event);
		void				mouseReleaseEvent(QMouseEvent *event);
		void				wheelEvent(QWheelEvent *event);
	

	private:
	
		void				initLighting();
		
		double				gscale;	
		double				mAspectRatio;
		QPoint				lastPos;
		QPoint				offset;
		bool				mStartSimulation;
		bool				gstepmode;	
		bool				mSimStepMode;
		bool				mBoolForSimControl;
		/*GLuint				backgroundDL;*/
		bool				mIsNewFile;
	//////////////////////
		ViewHandle			mViewHandle;
		GLuint				mPickBuffer[PICK_BUFFER_SIZE];
		
		QPoint				mMouseDownPoint;
	};
}

#endif