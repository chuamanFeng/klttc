#pragma once

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui_c.h"
#include "opencv2\highgui\highgui.hpp"
#include <iostream>

using namespace cv;
using namespace std;
;

class Rectchoose
{
public:
	Rectchoose(void);
	virtual ~Rectchoose(void);
public:
	enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 };

public:
	void mouseClick( int event, int x, int y/*, int flags, void* param */);
	void setImageAndWinName( const Mat& _image, const string& _winName);
	void reset();
	void showImage() const;

public:
	Rect ROI;
	//uchar rectState;
	bool select_flag;
	const Mat* image;
	Mat mask;
	const string* winName;
};

static void mouser(int event, int x, int y, int, void* this_) {
	static_cast<Rectchoose*>(this_)->mouseClick(event, x, y);
}
