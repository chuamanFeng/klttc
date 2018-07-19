#pragma once


// on_mouse2



#include <opencv2/core/core.hpp>
#include "opencv2\highgui\highgui_c.h"
#include "opencv2\highgui\highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;

class on_mouse2 : public CWnd
{
	DECLARE_DYNAMIC(on_mouse2)

public:
	on_mouse2(Mat frame/*,UINT id*/,string w,CvSize SZ);
	virtual ~on_mouse2();
public:
	bool  select_flag;
	Point corner1,corner2;
	Mat A;
		
	string wind;
	CvSize siz;	
	Rect ROI;
public:
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	void choose_ROI(int event, int x, int y,int flags);	
	Rect drawit();
};

static void mouser(int event, int x, int y, int flags, void* this_) {
	static_cast<on_mouse2*>(this_)->choose_ROI(event, x, y,flags);
}
