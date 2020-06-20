#ifndef ELLIPSE_DETECTOR_H
#define ELLIPSE_DETECTOR_H

#include <iostream>
#include <math.h>
#include <time.h>
#include <opencv2/opencv.hpp>
#include <vector>
//#include <cv.h>
//#include <highgui.h>



#define CONNECTION_ANGLE_THRES 30
#define CONNECTION_THRES 10

//using namespace cv;

void detect_ellipse(cv::Mat image, std::vector<cv::Vec4i> lines, std::vector<cv::Vec4i> &ellipse_prob_lines);

#endif