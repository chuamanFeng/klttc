// KLTcalc2.cpp : 实现文件
//

#include "stdafx.h"
#include "KLT.h"
#include "KLTcalc2.h"
#include "CvvImage.h"


// KLTcalc2

IMPLEMENT_DYNAMIC(KLTcalc2, ::CWnd)

KLTcalc2::KLTcalc2(Mat& video,Mat& src,Rect roi,UINT id,int max,double level,double mindis)
{
	ROI=roi;
	src.copyTo(Initialroi);//src is the latest graph in the frame
	maxCount = max;
	qLevel =level;
	minDist=mindis;
   ID=id;
	
}

KLTcalc2::~KLTcalc2()
{
}


BEGIN_MESSAGE_MAP(KLTcalc2, CWnd)
END_MESSAGE_MAP()



// KLTcalc2 消息处理程序


bool KLTcalc2::addNewPoints()
{
	return points[0].size() <= 10;
}


bool KLTcalc2::acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
}


bool KLTcalc2::calculate4init()
{
	if(!Initialroi.empty())
	{
		Mat temp;		
		cvtColor(Initialroi, temp, CV_BGR2GRAY);
		Mat INI(temp,ROI);  
		//frame.copyTo(output);

		if (addNewPoints())
		{
			goodFeaturesToTrack(INI, features, maxCount, qLevel, minDist);
			//points[0].insert(points[0].end(), features.begin(), features.end());
			initial.insert(initial.end(), features.begin(), features.end());
		}
		for (size_t i=0; i<initial.size(); i++)
		{
			circle(Initialroi, initial[i], 3, Scalar(0, 255, 0), -1);
		}
		//imshow(window_name,Initialroi);
		Display(&IplImage(Initialroi),ID);
		AfxMessageBox(_T("key points ok"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("no initialimg"));
		return false;
	}


}


void KLTcalc2::traking(Mat &frame, Mat &output)
{
	cvtColor(frame, gray, CV_BGR2GRAY);
	frame.copyTo(output);

	if(points[0].empty())//it's the second frame coming
	{
		points[0].insert(points[0].end(), initial.begin(), initial.end());
		Initialroi.copyTo(gray_prev);
	}
	else
	{
		if (addNewPoints())
		{
			goodFeaturesToTrack(gray, features, maxCount, qLevel, minDist);
			points[0].insert(points[0].end(), features.begin(), features.end());
			initial.insert(initial.end(), features.begin(), features.end());
		}

		if (gray_prev.empty())
		{
			gray.copyTo(gray_prev);
		}
	}

	calcOpticalFlowPyrLK(gray_prev, gray, points[0], points[1], status, err);

	int k = 0;
	for (size_t i=0; i<points[1].size(); i++)
	{
		if (acceptTrackedPoint(i))
		{
			initial[k] = initial[i];
			points[1][k++] = points[1][i];
		}
	}
	points[1].resize(k);
	initial.resize(k);

	for (size_t i=0; i<points[1].size(); i++)
	{
		line(output, initial[i], points[1][i], Scalar(0, 0, 255));
		circle(output, points[1][i], 3, Scalar(0, 255, 0), -1);
	}

	swap(points[1], points[0]);
	swap(gray_prev, gray);

	//imshow(window_name, output);
	Display(&IplImage(output),ID);
}

void KLTcalc2::Display(IplImage * img,UINT ID)
{
	CDC* pDC = GetDlgItem(ID)->GetDC(); 
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;  
	CWnd  *pWnd;  

	int picWidth;  
	int picHeight;  

	pWnd = GetDlgItem(ID);  
	pWnd->GetClientRect(&rect);  


	picWidth = rect.Width();  
	picHeight = rect.Height();  

	IplImage*dst = cvCreateImage(cvSize(picWidth, picHeight), img->depth, img->nChannels); 
	cvResize(img, dst, CV_INTER_LINEAR);  


	CvvImage cimg;  
	cimg.CopyOf(dst); 	
	cimg.DrawToHDC(hDC, &rect);
	cvReleaseImage(&dst); 
	ReleaseDC(pDC);  

}
