//#ifndef NC_VIDEOCAPTURE
//#define NC_VIDEOCAPTURE
//
//#include "NcUIComponents\NcUIComponentsGlobal.h"
//
//#include <QLabel>
//#include <QImage>
//#include <QTimer>
//
//namespace DiscreteSimulator
//{
//	
//	class NCUICOMPONENTSEXPORT NcVideoCapture : public QLabel
//	{
//		Q_OBJECT
//	public:
//		static NcVideoCapture* NcVideoCaptureInstance();
//		virtual ~NcVideoCapture();
//	public slots:
//		void startCapture();
//		void stopCapture();
//
//	private slots:
//			void capture();
//	private:
//		NcVideoCapture(QWidget *parent = 0);
//		NcVideoCapture(const NcVideoCapture&);
//		NcVideoCapture& operator=(const NcVideoCapture&);
//		
//		/*QImage iplImage2QImage(const IplImage *iplImage);*/
//	private:
//		static NcVideoCapture*	mNcVideoCaptureInstance;
//		class VideoCapturePrivate* d_ptr;
//		QTimer*		mTimer;
//	};
//}
//
//
//#endif