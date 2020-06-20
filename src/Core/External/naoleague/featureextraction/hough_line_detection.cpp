#include <iostream>
#include <math.h>
#include <time.h>
//#include <cv.h>
//#include <highgui.h>
#include "hough_line_detection.h"

//using namespace cv;

void probabilistic_hough_trans(cv::Mat src, vector<cv::Vec4i> &lines)
{
	cv::Mat dst, color_dst;
	// edge detection with canny edge detector
	// is applied...
	Canny( src, dst, 50, 200, 3 );
	// color image to gray scale...
	cvtColor( dst, color_dst, CV_GRAY2BGR );
	// hough probabilistic transformation
	HoughLinesP( dst, lines, 1, CV_PI/1000, 10, 10, 10 );
	return;
}
