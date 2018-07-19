#pragma once

#include "opencv2/core/core.hpp"

using namespace cv;

typedef struct vecmsg
{
	int pn1,pn2;
	Point2f vec;
	float mol;
}vecmsg;

typedef struct mat_roi_tm
{
	Mat frame;
	Rect ROI;
	float tm;
}mat_roi_tm;