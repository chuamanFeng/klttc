#pragma once


#include "gdiplustypes.h"
#include <iostream>
#include "opencv2/core/core.hpp"
#include "afxwin.h"
#include "opencv2/highgui/highgui_c.h"
#include "structs.h"
#include "objectFind.h"

using namespace std;
using namespace cv;
//using namespace Eigen;
/*----------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
class datatrans
{
public:
	datatrans(bool svm,double minarea,double maxarea,double c1,double T,Mat camMatrix,Mat distCoeff,int he,int wi);
	~datatrans(void);
public:
	
	vector<Point2f> centers;
	vector<Mat> contours;
	vector<Mat_<float>> pos_res;
	double minArea,maxArea;
	double circleli,Tr;
	Mat cam,disc;
	vector<Point3f> OBj;
	int ok,no;
	int count4sq;
	vector<Mat> error_tp;
	int h,w;
	
	Ptr<FeatureDetector> featureDetector;	
	Ptr<BOWImgDescriptorExtractor> bowDescriptorExtractor;
	
	int categories;
	vector<string> category_names;
	map<string, Ptr<CvSVM> >svms;
	bool svmFLAG;
public:	
	void imgPros(Mat res,Rect roi);
	void imgProssvm(Mat res,Rect roi);
	bool categorize(Mat& gray);
	//image pros functions
	Mat performThreshold(Mat res);	
	Point2f getcenter(Mat src,Point2f ORG,Mat& contour);
	//functions for math calculations
	Point2f vec(Point2f a,Point2f b);
	Point2f transorg(int x0,int y0,Point2f prt);

	float sinval(Point2f v1,Point2f v2);	
	float cosval(Point2f v1,Point2f v2);
	float mol(Point2f vec);

	bool circlelike(double polyarea,float R);	

	void vec4cal(double a,vector<double> cool);
	void inv(Mat_<float> a);

	int relats_bt_centers(vector<Point2f>& cts);

	Mat_<float> solve_pnp(vector<Point2f> recog_cts2);
	
	vector<Point> mat2vec(Mat& t1f);

	
};

bool compar1(pair<int,double>& a,pair<int,double>& b);