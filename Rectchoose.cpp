
#include "StdAfx.h"
#include "Rectchoose.h"
#include "KLT.h"
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/core/core.hpp>
#include "CvvImage.h"

#define GC_BGD 0
#define GC_FGD 1
#define GC_PR_BGD 2
#define GC_PR_FGD 3
const Scalar GREEN = Scalar(0,255,0);

Rectchoose::Rectchoose(void)
{
	select_flag =false;
	ROI.x=ROI.y=0;
}


void Rectchoose::reset()
{
	if( !mask.empty() )
		mask.setTo(Scalar::all(GC_BGD));
	//select_flag =false;
}


void Rectchoose::setImageAndWinName( const Mat& _image, const string& _winName  )
{
	if( _image.empty() || _winName.empty() )
		return;
	image = &_image;
	winName = &_winName;
	mask.create( image->size(), CV_8UC1);
	reset();
}

void Rectchoose::showImage() const
{
	if( image->empty() || winName->empty() )
		return;

	Mat res;
	image->copyTo( res );

	if( select_flag)
		rectangle( res, Point( ROI.x, ROI.y ), Point(ROI.x + ROI.width,ROI.y + ROI.height ), GREEN, 2);

	imshow( *winName, res );
}


void Rectchoose::mouseClick( int event, int x, int y/*, int flags, void* */)
{
	switch( event )
	{
	case  EVENT_LBUTTONDOWN:  
		{  
			ROI.x = x;  
			ROI.y = y;  
			select_flag = true;  
		  
			showImage();
		}
		break;
	case CV_EVENT_MOUSEMOVE:
		if(select_flag)
		{
			ROI= Rect( Point(ROI.x, ROI.y), Point(x,y) );
			showImage();
		}
		break;
	case CV_EVENT_LBUTTONUP:
		if( select_flag )
		{
			ROI = Rect( Point(ROI.x, ROI.y), Point(x,y) );
			select_flag= false;
			showImage();
		}
		break;
	}
}


Rectchoose::~Rectchoose(void)
{
}
