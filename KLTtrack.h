#pragma once
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "structs.h"

using namespace cv;
using namespace std;

class KLTtrack
{
public:
	KLTtrack(int count,double level,double mindist);
	~KLTtrack(void);
public:
	bool calculate4init(Mat& src,UINT ID,Rect roi);
	void traking(Mat &frame,Mat &output/*,UINT ID*/);
	bool acceptTrackedPoint(int i);
	bool addNewPoints();
	Point2f ctboundingbox(vector<Point2f>& ptss/*,int height,int width*/);
	vector<Point2f> transorg(int x0,int y0,vector<Point2f> prt);

	int maxCount;	
	double qLevel;	
	double minDist;	

	vector<uchar> status;	
	vector<float> err;	
	vector<mat_roi_tm> res;

	Mat gray;	
	Mat gray_prev;
	//Mat output;

	Rect ROI;

	vector<Point2f> points[2];
	vector<Point2f> initial;
	vector<Point2f> features;
};

