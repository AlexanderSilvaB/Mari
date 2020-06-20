#ifndef GOAL_DETECTION_H
#define GOAL_DETECTION_H

#include <iostream>
#include <math.h>
#include <time.h>
//#include <cv.h>
//#include <highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define R_POST 0
#define L_POST 1
#define V_POST 2
#define H_POST 3

#define HIST_THRESHOLD 0.1
#define ROOT_GAIN 1.5
#define CONTROL_MAX 0.2
#define CROP_THRESHOLD 5
#define SAMPLING_VER 5
#define SAMPLING_HOR 5
#define ROOT_OFFSET_Y 10

//using namespace cv;

struct goalposts{
	int type;
	cv::Vec4i line;
	cv::Point root_position;
	cv::Point top_position;
	double width;
	double root_confidence;
};

struct posts_lines{
	int candidate;
	cv::Vec4i line;
};

void goalPostDetection(cv::Mat image, std::vector<cv::Point> goalRoots, double* hor_hist, int* ver_hist, std::vector<goalposts> &goalPosts);

#endif