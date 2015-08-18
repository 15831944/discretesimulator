//#include <QTimer>
//#include <QDebug>
//#include "opencv2/highgui/highgui.hpp"
//#include "NcUIComponents\NcVideoCapture.h"
//using namespace DiscreteSimulator;
//using namespace cv;
//
//namespace DiscreteSimulator
//{
//	class VideoCapturePrivate
//	{
//	public:
//		CvCapture *mCapture;
//		IplImage  *mFrame;
//	};
//}
//NcVideoCapture* NcVideoCapture::mNcVideoCaptureInstance = 0;
//
//NcVideoCapture::NcVideoCapture(QWidget *parent) 
//: QLabel(parent),d_ptr(new VideoCapturePrivate() )
//{
//	
//	/* initialize camera */
//    d_ptr->mCapture = cvCaptureFromCAM( 0 );
// 
//	d_ptr->mFrame=0;
//	mTimer = new QTimer(this);
//	connect(mTimer, SIGNAL(timeout()), this, SLOT(capture()));;
//}
//
//NcVideoCapture::~NcVideoCapture()
//{
//	cvReleaseCapture( &d_ptr->mCapture );
//}
//
//void NcVideoCapture::startCapture()
//{
//	mTimer->start(2);
//}
//
//void NcVideoCapture::stopCapture()
//{
//	mTimer->stop();
//}
//
//void NcVideoCapture::capture()
//{
//
//	 /* always check */
//    if ( !d_ptr->mCapture ) {
//        return ;
//    }
//    /* get a frame */
//    d_ptr->mFrame = cvQueryFrame( d_ptr->mCapture );
//	  /* always check */
//    if( !d_ptr->mFrame ) return;
//
//	/*QImage qimage=iplImage2QImage(d_ptr->mFrame);*/
//	QImage qimage;
//	{
//		const IplImage* iplImage =d_ptr->mFrame;
//		int height = iplImage->height;
//		int width = iplImage->width;
//	 
//		if  (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 3)
//		{
//			  const uchar *qImageBuffer = (const uchar*)iplImage->imageData;
//			  QImage img(qImageBuffer, width, height, QImage::Format_RGB888);
//			  qimage = img.rgbSwapped();
//		} else if  (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 1){
//			const uchar *qImageBuffer = (const uchar*)iplImage->imageData;
//			QImage img(qImageBuffer, width, height, QImage::Format_Indexed8);
//		 
//			QVector<QRgb> colorTable;
//			for (int i = 0; i < 256; i++){
//				colorTable.push_back(qRgb(i, i, i));
//			}
//			img.setColorTable(colorTable);
//			qimage=img;
//		}else{
//		  qWarning() << "Image cannot be converted.";
//		  qimage=QImage();
//		}
//	}
//
//
//	if (qimage.isNull()) {
//       return;
//    }
//	setPixmap(QPixmap::fromImage(qimage));
//	adjustSize();
//
//
//}
//
////QImage NcVideoCapture::iplImage2QImage(const IplImage *iplImage)
////{
////    int height = iplImage->height;
////    int width = iplImage->width;
//// 
////    if  (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 3)
////    {
////		  const uchar *qImageBuffer = (const uchar*)iplImage->imageData;
////		  QImage img(qImageBuffer, width, height, QImage::Format_RGB888);
////		  return img.rgbSwapped();
////    } else if  (iplImage->depth == IPL_DEPTH_8U && iplImage->nChannels == 1){
////		const uchar *qImageBuffer = (const uchar*)iplImage->imageData;
////		QImage img(qImageBuffer, width, height, QImage::Format_Indexed8);
////	 
////		QVector<QRgb> colorTable;
////		for (int i = 0; i < 256; i++){
////			colorTable.push_back(qRgb(i, i, i));
////		}
////		img.setColorTable(colorTable);
////	return img;
////    }else{
////      qWarning() << "Image cannot be converted.";
////      return QImage();
////    }
////}
////
//
//NcVideoCapture* NcVideoCapture::NcVideoCaptureInstance()
//{
//	if(NcVideoCapture::mNcVideoCaptureInstance == 0)
//	{
//		NcVideoCapture::mNcVideoCaptureInstance = new NcVideoCapture();
//	}
//	return NcVideoCapture::mNcVideoCaptureInstance;
//}
