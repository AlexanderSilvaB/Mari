#ifndef HOUGH_LINE_DETECTION_H
#define HOUGH_LINE_DETECTION_H

#include <iostream>
#include <math.h>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <vector>


//#include <cv.h>
//#include <highgui.h>
//#include "opencv2/imgproc.hpp"

using namespace std;
//using namespace cv;

void probabilistic_hough_trans(cv::Mat src, std::vector<cv::Vec4i> &lines);

#endif
