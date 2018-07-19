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
	void Display(IplImage * img,UINT ID);

protected:
	DECLARE_MESSAGE_MAP()
};


