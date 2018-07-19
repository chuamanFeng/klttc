
// KLTDlg.h : 头文件
//

#pragma once
#include "gdiplustypes.h"
#include <iostream>
#include "opencv2/core/core.hpp"
#include "afxwin.h"
#include "opencv2/highgui/highgui_c.h"
#include "structs.h"

using namespace std;
using namespace cv;

// CKLTDlg 对话框

class CKLTDlg : public CDialogEx
{
// 构造
public:
	CKLTDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_KLT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
/*	CTimeMeasure time;
	LONGLONG tm;
	*/
	Rect roi;	
	CEvent start_event;
	CButton pausebutton;
	int terminate_flag,calculatflag,flagKLT;
	bool flagSVM;
	int H,W,H1,W1;
	CWinThread* mythread;
	Mat result;
	Mat showframe;
	Mat frame4klt;
	Mat camera,discoeff;//we shall initial it  later
	

	afx_msg void OnBnClickedstart();
	afx_msg void OnBnClickedpause();
	afx_msg void OnBnClickedstop();
	afx_msg void OnBnClickedtracking();
	afx_msg void OnBnClickedShowroi();
	afx_msg void OnBnClickedcalculate();
	afx_msg void OnBnClickeddatatrans();
	afx_msg void OnBnClickedChecksvm();

	

	bool calculate4init(Mat& src,UINT ID);	
	mat_roi_tm traking(int h1,int w1,float tm);
	bool acceptTrackedPoint(int i);
	bool addNewPoints();
	
	Mat Initialroi;
	Mat gray;	
	Mat gray_prev;	

    vector<Point2f> points[2];
	vector<Point2f> initial;
	vector<Point2f> features;
	
	int maxCount/* = 30*/;	
	double qLevel/* = 0.01*/;	
	double minDist/* = 10.0*/;	
 
    vector<uchar> status;	
	vector<float> err;
	vector<mat_roi_tm> res; 


	static DWORD WINAPI PLAYER(LPVOID lparam);
	static DWORD WINAPI DRAWER(LPVOID pparam);
	static DWORD WINAPI KLTMAKER(LPVOID param);
	static DWORD WINAPI CALC(LPVOID param);


	vector<Point2f> transorg(int x,int y,vector<Point2f> prt);
	void Display(IplImage * img,UINT ID);
	CvSize getPICsize(UINT ID);		
    Rect boundingbox(vector<Point2f> ptss,int height,int width);
	
	

	
	
};


