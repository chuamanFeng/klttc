#pragma once


// KLTcalc2

#pragma once
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>


using namespace std;
using namespace cv;


class KLTcalc2 : public CWnd
{
	DECLARE_DYNAMIC(KLTcalc2)

public:
	KLTcalc2(Mat& video,Mat& src,Rect roi,UINT id,int max,double level,double mindis);
	virtual ~KLTcalc2();
public:
	//UINT ID;
	Rect ROI;
	Mat Initialroi;
	Mat gray;	// 当前图片
	Mat gray_prev;	// 预测图片
	vector<Point2f> points[2];	// point0为特征点的原来位置，point1为特征点的新位置
	vector<Point2f> initial;	// 初始化跟踪点的位置
	vector<Point2f> features;	// 检测的特征
	int maxCount/* = 500*/;	// 检测的最大特征数
	double qLevel/* = 0.01*/;	// 特征检测的等级
	double minDist/* = 10.0*/;	// 两特征点之间的最小距离
	vector<uchar> status;	// 跟踪特征的状态，特征的流发现为1，否则为0
	vector<float> err;
public:
	/*void tracking(Mat &frame, Mat &output);
	bool addNewPoints();
	bool acceptTrackedPoint(int i);*/
	
	bool addNewPoints();
	bool acceptTrackedPoint(int i);
	bool calculate4init();
	void traking(Mat &frame, Mat &output);
	void Display(IplImage * img,UINT ID);

protected:
	DECLARE_MESSAGE_MAP()
};


