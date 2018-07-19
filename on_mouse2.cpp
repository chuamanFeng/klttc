// on_mouse2.cpp : 实现文件
//

#include "stdafx.h"
#include "KLT.h"
#include "on_mouse2.h"
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/core/core.hpp>
#include "CvvImage.h"

using namespace std;
using namespace cv;

// on_mouse2

IMPLEMENT_DYNAMIC(on_mouse2, CWnd)

on_mouse2::on_mouse2(Mat frame/*,UINT id*/,string w,CvSize SZ)
{
	select_flag =false;
	wind=w;
	frame.copyTo(A);	
	//ID=id;
	siz=SZ;
}

on_mouse2::~on_mouse2()
{
}


BEGIN_MESSAGE_MAP(on_mouse2, CWnd)
END_MESSAGE_MAP()



// on_mouse2 messageMap



void  on_mouse2::choose_ROI(int event, int x, int y,int flags)
{	
	Mat b,tem;
	A.copyTo(b);
	if (event == CV_EVENT_LBUTTONDOWN)
	{  	
		corner1 = Point(x,y);  		
		circle(b,corner1,2,Scalar(255,0,0,0),CV_FILLED,CV_AA,0);  
		imshow(wind,b);  
	}  
	else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))
	{  
		b.copyTo(tem);		
		corner2= Point(x,y);  		
		imshow(wind,tem);  
	}  
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
	{  
		b.copyTo(tem);
		corner2 = Point(x,y);  		
		rectangle(tem,corner1,corner2,Scalar(0,255,0,0),4,8,0);
		imshow(wind,tem);  
	}  
	else if (event == CV_EVENT_LBUTTONUP)
	{  		
		corner2 = Point(x,y);  		
		circle(b,corner1,2,Scalar(255,0,0,0),CV_FILLED,CV_AA,0);  
		rectangle(b,corner1,corner2,Scalar(0,255,0,0),4,8,0);
		imshow(wind,b);  
		b.copyTo(tem);  
		
		int width = abs(corner1.x - corner2.x);  
		int height = abs(corner1.y - corner2.y);  
		if (width == 0 || height == 0)  
		{  
			printf("width == 0 || height == 0");  
			return;  
		}  
		ROI.x=corner1.x;
		ROI.y=corner1.y;
		ROI.height=height;
		ROI.width=width;
	
		waitKey(0);  
	}  
}  


Rect on_mouse2::drawit()
{	
	cvNamedWindow( wind.c_str(),0);
	cvResizeWindow( wind.c_str(),siz.width,siz.height);
	imshow( wind.c_str(),A);

	while(1)
	{
		int key = waitKey(10); 
		
		setMouseCallback(wind, mouser, this);
		
		
		if (ROI.area()!=0) 
		{
			AfxMessageBox(_T("OK"));
			break;
		}
	}
	
	waitKey(0); 
	return ROI;
}  

	