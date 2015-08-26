#ifndef NC_TOOLPATHGLWIDGET
#define NC_TOOLPATHGLWIDGET

#include <QGLWidget>
#include "NcUiComponents\NcUIComponentsGlobal.h"


class QWidget;
namespace DiscreteSimulator
{

	class NCUICOMPONENTSEXPORT NcToolPathWindow : public QGLWidget
	{
		Q_OBJECT
		
	public:
		NcToolPathWindow(QWidget *parent = 0, QGLWidget *shareWidget = 0);

		void		initializeGL();
		/*virtual		~NcToolPathWindow();*/
		void		updateToolPathWindow();
		void		reinitializeSimVariables();
		
	public slots:
		
		void		updateOperation(int);

	protected:
		void		paintGL();			
		void		resizeGL(int width,int height);
		void		mousePressEvent(QMouseEvent *event);
		void		mouseMoveEvent(QMouseEvent *event);
		void		mouseReleaseEvent(QMouseEvent *event);
		void		wheelEvent(QWheelEvent *event);

	private:
	
		void		setOrthographicProjection();

	private:
		double		xRot;
		double		yRot, zRot;
		double		gscale;
		QColor		mBackGroundColor;
		QPoint		lastPos;
		QPoint		offset;
		double		mAspectRatio;
		int			op_u;
		bool		mStartSimulation;

	};
}

#endif