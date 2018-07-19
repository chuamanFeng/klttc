#include "StdAfx.h"
#include "KLTtrack.h"
#include "opencv2/opencv.hpp"
#include "CvvImage.h"
#include "opencv2/imgproc/imgproc_c.h"

using namespace cv;
using namespace std;




KLTtrack::KLTtrack(int count,double level,double mindist)
{
	maxCount=count;	
	qLevel=level;	
	minDist =mindist;	
	
}
/*
bool KLTtrack::calculate4init(Mat& src,UINT ID,Rect roi)
{
	Mat Initialroi;
	src.copyTo(Initialroi);
	int orgx,orgy;
	orgx=roi.x;
	orgy=roi.y;
	if(!Initialroi.empty())
	{
		Mat tmp;		
		cvtColor(Initialroi, tmp, CV_BGR2GRAY);
		Mat INI(tmp,roi);  


		if (addNewPoints())
		{
			goodFeaturesToTrack(INI, features, maxCount, qLevel, minDist);

			features=transorg(orgx,orgy,features);
			points[0].insert(points[0].end(), features.begin(), features.end());
			initial.insert(initial.end(), features.begin(), features.end());
		}
		for (size_t i=0; i<initial.size(); i++)
		{
			circle(Initialroi, initial[i], 3, Scalar(0, 255, 0), -1);
		}

		//Display(&IplImage(Initialroi),ID);
		//AfxMessageBox(_T("key points ok"));
		return true;
	}
	else
	{
		AfxMessageBox(_T("no initialimg"));
		return false;
	}
}
*/
void KLTtrack::traking(Mat &frame,Mat &output)
{
	cvtColor(frame, gray, CV_BGR2GRAY);
	frame.copyTo(output);
	if(!initial.empty()&&points[0].empty())
	{
		points[0].insert(points[0].end(),initial.begin(),initial.end());
		initial.swap(initial);
	}
		
	if (addNewPoints())
	{
		goodFeaturesToTrack(gray, features, maxCount, qLevel, minDist);
		points[0].insert(points[0].end(), features.begin(), features.end());
		//initial.insert(initial.end(), features.begin(), features.end());
	}
	
	if (gray_prev.empty())
	{
		gray.copyTo(gray_prev);
	}
	//}	
	calcOpticalFlowPyrLK(gray_prev, gray, points[0], points[1], status, err);	
	
	int k = 0;
	for (size_t i=0; i<points[1].size(); i++)
	{
		if (acceptTrackedPoint(i))
		{
			//initial[k] = initial[i];
			points[1][k] = points[1][i];
			k++;
		}
	}
	
	points[1].resize(k);
	//initial.resize(k);
	
	//ROI=Rect()
	//rectangle(output,Point(cent.x,cent.y),Point(int(cent.x+gray.cols/4),int(cent.y-gray.rows/4)),Scalar(255, 0, 0),2);
	
	for (size_t i=0; i<points[1].size(); i++)
	{
		line(output,points[0][i], points[1][i], Scalar(0, 0, 255));
		circle(output, points[1][i], 3, Scalar(0, 255, 0), -1);
		
	}
	
	swap(points[1], points[0]);	
	swap(gray_prev, gray);
//	imshow(wdn,output);
	
}


bool KLTtrack::acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
}

bool KLTtrack::addNewPoints()
{
	return points[0].size() <=5;
}


vector<Point2f> KLTtrack::transorg(int x0,int y0,vector<Point2f> prt)
{
	vector<Point2f>::iterator it=prt.begin();
	while(it!=prt.end())
	{
		(*it).x+=x0;
		(*it).y+=y0;
		it++;
	}
	return prt;
}

Point2f KLTtrack::ctboundingbox(vector<Point2f>& ptss/*,int height,int width*/)
{
	vector<Point2f> pts;
	pts.insert(pts.end(),ptss.begin(),ptss.end());
	vector<Point2f>::iterator it=pts.begin();
	float xo=0.f,yo=0.f;
	int sz=pts.size();

	while(it!=pts.end())
	{
		xo+=(*it).x;
		yo+=(*it).y;
		it++;
	}	
	Point2f targ=Point2f(xo/sz,yo/sz);
	/***************************************/
	float tEemp=0.f,temp=0.f, aver_Edist=0.f,aver_dist=0.f,temp1=0.f;
	vector<float> TMP,TMP1;
	int i,j;
	for(i=0;i<sz;i++)
	{
		for(j=i+1;j<sz;j++)
		{
			temp1=sqrt((pts[i].x-pts[j].x)*((pts[i].x-pts[j].x))+(pts[i].y-pts[j].y)*((pts[i].y-pts[j].y)));
			tEemp+=temp1;
		}
		TMP.push_back(temp1/(sz-i));

		temp1=sqrt((targ.x-pts[i].x)*(targ.x-pts[i].x)+(targ.y-pts[i].y)*(targ.y-pts[i].y));
		TMP1.push_back(temp1);
		temp+=temp1;

	}
	aver_dist=(float)(1.2*temp/sz);//average dist to the center
	aver_Edist=(float)(1.2*tEemp/(TMP.size()));//average dis to each other
	/***************************************/
	//vector<Point2f> useful;
	vector<float>::iterator it1=TMP.begin();
	vector<float>::iterator it2=TMP1.begin();
	it=pts.begin();

	while(it!=pts.end())
	{		
		if(*it2>=aver_dist||*it1>=aver_Edist)
		{
			//useful.push_back(*it);
			pts.erase(it);
			it--;
		}
		it++;
		it1++;
		it2++;
	}
	it=pts.begin();
	int sz1=pts.size();
	tEemp=0.f,temp=0.f;
	for(int i=0;i<sz1;i++)
	{
		tEemp+=pts[i].x;
		temp+=pts[i].y;
	}
	Point2f cen=Point2f(tEemp/sz1,temp/sz1);
		
	/**************************************/
	/*Rect target((int)(cen.x-width/8),(int)(cen.y+height/8),width/4,height/4);
	//target=boundingRect(pts);
	if(target.x<0)
	{
		target.x=0;
	}
	if(target.y<0)
	{
		target.y=0;
	}
	if(target.y+target.height>height)
	{
		target.height=height-target.y;
	}
	if(target.x+target.width>width)
	{
		target.width=width-target.x;
	}*/
	return cen;
}

KLTtrack::~KLTtrack(void)
{
}
