#pragma once
#include <opencv2/video/video.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>


using namespace std;
using namespace cv;

class KLTcalc
{
public:
	KLTcalc(string window,Mat& src,Rect roi,int max,double level,double mindis);
	virtual ~KLTcalc(void);
public:
	//string window_name /*= "optical flow tracking"*/;
	Rect ROI;
	Mat Initialroi;
	Mat gray;	// ��ǰͼƬ
	Mat gray_prev;	// Ԥ��ͼƬ
	vector<Point2f> points[2];	// point0Ϊ�������ԭ��λ�ã�point1Ϊ���������λ��
	vector<Point2f> initial;	// ��ʼ�����ٵ��λ��
	vector<Point2f> features;	// ��������
	int maxCount/* = 500*/;	// �������������
	double qLevel/* = 0.01*/;	// �������ĵȼ�
	double minDist/* = 10.0*/;	// ��������֮�����С����
	vector<uchar> status;	// ����������״̬��������������Ϊ1������Ϊ0
	vector<float> err;
public:
	/*void tracking(Mat &frame, Mat &output);
	bool addNewPoints();
	bool acceptTrackedPoint(int i);*/
	
	bool addNewPoints();
	bool acceptTrackedPoint(int i);
	bool calculate4init();
	void traking(Mat &frame, Mat &output);
};

