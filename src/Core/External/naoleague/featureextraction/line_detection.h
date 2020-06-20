#ifndef LINE_DETECTION_H
#define LINE_DETECTION_H

#include <iostream>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <vector>


//#include <cv.h>
//#include <highgui.h>

//using namespace cv;

struct point_dis
{
	cv::Point pnt;
	double distance;
};

void line_extraction(cv::Mat image, std::vector<cv::Vec4i> &produced_lines, int hor_step, int ver_step);

#endif
