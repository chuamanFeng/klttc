#include "StdAfx.h"
#include "KLTcalc.h"


KLTcalc::KLTcalc(Mat& video,Mat& src,Rect roi,int max,double level,double mindis)
{
	//window_name=window;
	ROI=roi;
	src.copyTo(Initialroi);//src is the latest graph in the frame
	maxCount = max;
	qLevel =level;
	minDist=mindis;
}

KLTcalc::~KLTcalc(void)
{
}


bool KLTcalc::addNewPoints()
{
	return points[0].size() <= 10;
}


bool KLTcalc::acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
}


bool KLTcalc::calculate4init()
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
		AfxMessageBox(_T("key points ok"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("no initialimg"));
		return false;
	}

		
}


void KLTcalc::traking(Mat &frame, Mat &output)
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
}

