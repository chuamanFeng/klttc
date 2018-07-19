
// KLTDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KLT.h"
#include "KLTDlg.h"
#include "afxdialogex.h"
#include <opencv2/video/video.hpp>
#include "opencv2/opencv.hpp"
#include "CvvImage.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "datatrans.h"

#include "on_mouse2.h"
#include "TimeMeasure.h"
#include "choice4pause.h"
#include "notice4ROI.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace cv;

double minarea=625;
double maxarea=40000;
double circlelike=0.0;
double tr=16;


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();


	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持


protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CKLTDlg 对话框




CKLTDlg::CKLTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CKLTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKLTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_pause, pausebutton);
}

BEGIN_MESSAGE_MAP(CKLTDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_start, &CKLTDlg::OnBnClickedstart)
	ON_BN_CLICKED(IDC_pause, &CKLTDlg::OnBnClickedpause)
	ON_BN_CLICKED(IDC_stop, &CKLTDlg::OnBnClickedstop)
	ON_BN_CLICKED(IDC_tracking, &CKLTDlg::OnBnClickedtracking)	
	ON_BN_CLICKED(IDC_SHOWROI, &CKLTDlg::OnBnClickedShowroi)
	ON_BN_CLICKED(IDC_calculate, &CKLTDlg::OnBnClickedcalculate)
	ON_BN_CLICKED(IDC_datatrans, &CKLTDlg::OnBnClickeddatatrans)	
	ON_BN_CLICKED(IDC_CHECKSVM, &CKLTDlg::OnBnClickedChecksvm)

END_MESSAGE_MAP()



BOOL CKLTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);	

	//filenamev=string("avi.1");
	// inital
	maxCount=30;	
	qLevel= 0.01;	
	minDist = 10.0;	
	flagSVM=false;
	flagKLT=-1;
		
	CvSize SZ=getPICsize(IDC_show);
	H=SZ.height;
	W=SZ.width;

	return TRUE; 
}

void CKLTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CKLTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


HCURSOR CKLTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

////////////////////////////////////////*************button events****************/////////////////////////////////

////////////////////////////////////////////////////////////start button
void CKLTDlg::OnBnClickedstart()
{
	start_event.SetEvent();  
    mythread =AfxBeginThread((AFX_THREADPROC)PLAYER,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
	
}  

//////////////////////////////////////////////////////////////pause button
void CKLTDlg::OnBnClickedpause()
{
	CString buttonText;  
	pausebutton.GetWindowText(buttonText);  

	if (buttonText.Compare(_T("pause"))==0)  
	{  
		start_event.ResetEvent();  
		
		pausebutton.SetWindowTextW(_T("continue"));  
		choice4pause choi;
		if(choi.DoModal()==IDOK)
		{
			if(!showframe.empty())
			{
				result=showframe.clone();
				H1=result.rows;
				W1=result.cols;
				roi.x=roi.y=roi.width=roi.height=0;
				mythread =AfxBeginThread((AFX_THREADPROC)DRAWER,this,THREAD_PRIORITY_NORMAL,0,0,NULL);						
									
			}
		}
				
	}  
	else  
	{  
		start_event.SetEvent();  
		pausebutton.SetWindowText(_T("pause"));  
	}  

}

//////////////////////////////////////////////////////////////////////stop button
void CKLTDlg::OnBnClickedstop()
{
	
	terminate_flag = -1;  
	flagKLT=-1;
	
}

///////////////////////////////////////////////////////////////////tracking button
void CKLTDlg::OnBnClickedtracking()
{
	if(initial.size()!=0)
	{
		flagKLT=1;
		mythread=AfxBeginThread((AFX_THREADPROC)KLTMAKER,this,THREAD_PRIORITY_NORMAL,0,0,NULL);	
		

	}
	else
	{
		AfxMessageBox(_T("No enough data" ));
	}
}






/////////////////////////////////////////////////////////////////ROI button
void CKLTDlg::OnBnClickedShowroi()
{
	if(roi.area()!=0)
	{	
		Mat temp=result;
		rectangle(temp,Point(roi.x,roi.y),Point(roi.x+roi.width,roi.y+roi.height),Scalar(0,255,0),2);
		Display(&IplImage(temp),IDC_show);
		notice4ROI dlg;
		if(dlg.DoModal()!=IDOK)
		{
			result.release();
			roi.x=roi.y=roi.height=roi.width=0;
			return;
		}
		
	}
	else
	{
		AfxMessageBox(_T("No ROI data"));
		return;
	}
}

//////////////////////////////////////////////////////////////////calc button
void CKLTDlg::OnBnClickedcalculate()
{
	if(roi.area()!=0&&!result.empty())
	{
		flagKLT=0;
	//	points[0].clear();
		mythread =AfxBeginThread((AFX_THREADPROC)KLTMAKER,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
		
	}
	else
	{
		AfxMessageBox(_T("No enough data" ));
	}
}



void CKLTDlg::OnBnClickeddatatrans()
{	
	if(flagKLT=-1&&!res.empty())
	{		
		mythread =AfxBeginThread((AFX_THREADPROC)CALC,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
		
	}
	else
	{
		AfxMessageBox(_T("lack res data"));
	}
}

void CKLTDlg::OnBnClickedChecksvm()
{
	CButton *pCheckbox = (CButton*)GetDlgItem(IDC_CHECKSVM);  
	if(pCheckbox->GetCheck())  
	{
		flagSVM=true;
	}
	else  
	{
		flagSVM=false;
	}


}


///////////////////////////////////////***************THREADS*************///////////////////////////////////////

////////////////////////////////////////////////////////////////////////////player thread
DWORD WINAPI CKLTDlg::PLAYER(LPVOID lparam)
{
	CKLTDlg* pThis = (CKLTDlg*)lparam;  
	VideoCapture pCapture("12.MP4");
	CTimeMeasure time;
//	CvSize SZ=pThis->getPICsize(IDC_show);

	if (/*pCapture=NULL*/!pCapture.isOpened())  
	{  
		AfxMessageBox(_T("shadiao"));
		return -1;  
	}  
	while(true)
	{
		if(pThis->terminate_flag!=-1) 
		{  		
			WaitForSingleObject(pThis->start_event, INFINITE); 	
			pThis->start_event.SetEvent(); 
			pCapture>>(pThis->showframe);	


			if(pThis->flagKLT!=1)
			{						
				pThis->Display(&IplImage(pThis->showframe), IDC_show);


			}	
			else
			{							
				float tm=time.GetTime();
				pThis->res.push_back(pThis->traking(pThis->H1,pThis->W1,tm));				
				pThis->Display(&IplImage(pThis->frame4klt),IDC_show);


			}		
			Sleep(52);		
		}


		if(pThis->terminate_flag==-1)
		{
			pThis->terminate_flag = 0; 
			pThis->showframe.release();
			_endthreadex(0);  

		}
	}
	
			
	return 0;
}  
///////////////////////////////////////////////////////////////////////drawer thread
DWORD WINAPI CKLTDlg::DRAWER(LPVOID pparam)
{
	CKLTDlg* pThis = (CKLTDlg*)pparam; 
	Mat TEMP=pThis->result;

	CvSize SZ=pThis->getPICsize(IDC_show);
	//CvSize SZ;	
	const string winName = "DrawRegion";

	
	on_mouse2 ms2(TEMP,winName,SZ);
	pThis->roi=ms2.drawit();
	if(pThis->roi.area()!=0)
	{

		return 0;
	}
	else
	{
		return -1;
	}

}

/////////////////////////////////////////////////////////////////////////////KLT thread
DWORD WINAPI CKLTDlg::KLTMAKER(LPVOID param)
{
	CKLTDlg* pThis = (CKLTDlg*)param;  
	

	if (pThis->flagKLT==0)  
	{  
		pThis->calculate4init(pThis->result,IDC_show);
		return 0;
	}  
	
	else if(pThis->flagKLT)
	{		
		pThis->start_event.SetEvent();  
		pThis->pausebutton.SetWindowText(_T("pause"));  
		return 0;
	}
	else
	{
		return -1;
	}
	
}
//////////////////////////////////////////////////////////////////////////////////calc thread
DWORD WINAPI CKLTDlg::CALC(LPVOID param)
{
	
	CKLTDlg* pThis = (CKLTDlg*)param;  	
	
	vector<mat_roi_tm>::iterator it=pThis->res.begin();	

	datatrans datapro(pThis->flagSVM,minarea,maxarea,circlelike,tr,pThis->camera,pThis->discoeff,pThis->H,pThis->W);
	
	if(!pThis->flagSVM)
	{
		while(it!=pThis->res.end())	
		{			
			datapro.imgPros(it->frame,it->ROI);//PROBLEM ,can be skipped			
			it++;
		}
	}
	else
	{
		while(it!=pThis->res.end()&&datapro.count4sq!=-1)	
		{			
			datapro.imgProssvm(it->frame,it->ROI);//PROBLEM ,can be skipped			
			it++;
		}
	}
	
	if(datapro.ok==0)
	{
		AfxMessageBox(_T("suck"));
	}
	else
	{
		AfxMessageBox(_T("COOL"));
	}
	
	return 0;
}
////////////////////////////////////***************KLT functions************/////////////////////////////////////////

////////////////////////////////////////////////////////init func
bool CKLTDlg::calculate4init(Mat& src,UINT ID)
{
	Initialroi.release();
	initial.clear();
	points[0].clear();
	points[1].clear();
	features.clear();

	Initialroi=src;
	int orgx,orgy;
	orgx=roi.x;
	orgy=roi.y;

	if(!Initialroi.empty())
	{
		Mat tmp;		
		cvtColor(Initialroi, tmp, CV_BGR2GRAY);
		tmp.copyTo(gray);
		tmp.copyTo(gray_prev);
		Mat INI(tmp,roi);  
		

		if (addNewPoints())
		{
			goodFeaturesToTrack(INI, features, maxCount, qLevel, minDist);			
			features=transorg(orgx,orgy,features);
			points[0].insert(points[0].end(), features.begin(), features.end());
			initial.insert(initial.end(), features.begin(), features.end());
	    }
		
		calcOpticalFlowPyrLK(gray_prev, gray, points[0], points[1], status, err);
		
	    for (size_t i=0; i<initial.size(); i++)
	    {
		    circle(Initialroi, initial[i], 3, Scalar(0, 255, 0), -1);
	    }
	    swap(gray_prev, gray);
	
	    Display(&IplImage(Initialroi),ID);
	
		return true;
	}
	
	else
	{
		AfxMessageBox(_T("no initialimg"));
		return false;
	}
}

//////////////////////////////////////////////////////////////////////tracking func
mat_roi_tm CKLTDlg::traking(int h1,int w1,float tm)
{
	cvtColor(showframe, gray, CV_BGR2GRAY);
	showframe.copyTo(frame4klt);	
	
	if (addNewPoints())//this part so slow
	{
		features.clear();
		goodFeaturesToTrack(gray, features, maxCount, qLevel, minDist);
		points[0].insert(points[0].end(), features.begin(), features.end());
		initial.insert(initial.end(), features.begin(), features.end());
	}
	
	if (gray_prev.empty())
	{
		gray.copyTo(gray_prev);
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
		line(frame4klt, initial[i], points[1][i], Scalar(0, 0, 255));
		circle(frame4klt, points[1][i], 3, Scalar(0, 255, 0), -1);
	}

	Rect rec=boundingbox(points[0],h1,w1);	
	rectangle(frame4klt,Point(rec.x,rec.y),Point(rec.x+rec.width,rec.y+rec.height),Scalar(255, 0, 0),2);
	
	swap(points[1], points[0]);
	/*Rect rec=boundingbox(points[0],h1,w1);	
	rectangle(frame4klt,Point(rec.x,rec.y),Point(rec.x+rec.width,rec.y+rec.height),Scalar(255, 0, 0),2);*/
	swap(gray_prev, gray);

	mat_roi_tm temp;
	temp.frame=showframe;
	temp.tm=tm;
	temp.ROI=rec;
	return temp;
	
}

bool CKLTDlg::acceptTrackedPoint(int i)
{
	return status[i] && ((abs(points[0][i].x - points[1][i].x) + abs(points[0][i].y - points[1][i].y)) > 2);
}


bool CKLTDlg::addNewPoints()
{
	return initial.size() <=5;
}




///////////////////////////////////////****************other functions************************////////////////////////////

//////////////////////////////////////////////////////////////////display function
void CKLTDlg::Display(IplImage * img,UINT ID)
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
///////////////////////////////////////////////////////////////////////////reset coordination
vector<Point2f> CKLTDlg::transorg(int x0,int y0,vector<Point2f> prt)
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
//////////////////////////////////////////////////////////////////////////get size func
CvSize CKLTDlg::getPICsize(UINT ID)
{
	CvSize SZ;
	CRect rect;  
	CWnd  *pWnd;  

	pWnd = GetDlgItem(ID);  
	pWnd->GetClientRect(&rect);  


	SZ.width = rect.Width();  
	SZ.height = rect.Height();  

	return SZ;
}
/////////////////////////////////////////////////////////////////////get boNdingbox topleft

Rect CKLTDlg::boundingbox(vector<Point2f> ptss,int height,int width)
{
	vector<Point2f> pts;
	ptss.swap(pts);
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
	/**************************************/
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
	aver_dist=2*temp/sz;//average dist to the center
	aver_Edist=2*tEemp/(TMP.size());//average dis to each other
	/**************************************/
	vector<Point2f> useful;
	vector<float>::iterator it1=TMP.begin();
	vector<float>::iterator it2=TMP1.begin();
	it=pts.begin();

	while(it!=pts.end())
	{		
		if(*it2<=aver_dist&&*it1<=aver_Edist)
		{
			useful.push_back(*it);
		}
		it++;
		it1++;
		it2++;
	}
	/*************************************/
	float radio;
	Point2f centerc;
	minEnclosingCircle(useful,centerc,radio);
	Rect target=boundingRect(pts);

	float a=centerc.x;
	float b=centerc.y;
	double c=target.x+0.5*target.width;
	double d=target.y+0.5*target.height;
	
    target.x-=(int)((a-c)+0.5*radio);
	target.y-=(int)((b-d)+0.5*radio);
	target.height+=(int)(radio);
	target.width+=(int)(radio);

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
	}
	
	return target;
}






