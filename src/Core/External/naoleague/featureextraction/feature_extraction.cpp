#include <iostream>
#include <math.h>
#include <time.h>
//#include <cv.h>
//#include <highgui.h>
#include "img_processing.h"
#include "line_detection.h"
#include "line_feature_detection.h"
#include "ellipse_detector.h"
#include "goal_detection.h"
#include "feature_extraction.h"

//using namespace cv;
using namespace std;

void extract_features(cv::Mat img_rgb, vector<field_point> &result_intersections,
					  vector<goalposts> &goalPosts)
{
	cv::Mat img_hsv;
	cv::Mat img_lines_binary, img_posts_binary, img_ball_binary;

	cvtColor(img_rgb, img_hsv, CV_BGR2HSV);

	vector<cv::Point> goalRoots;
	double hor_hist[img_hsv.cols];
	int ver_hist[img_hsv.rows];
	remove_background(img_hsv, img_lines_binary, img_posts_binary,
					  img_ball_binary, goalRoots, hor_hist, ver_hist);

	vector<cv::Vec4i> lines;
	line_extraction(img_lines_binary, lines, 5, 5);
	vector<cv::Vec4i> ellipse_prob_lines;
	detect_ellipse(img_lines_binary, lines, ellipse_prob_lines);

	line_most_prob_features(img_lines_binary, lines, ellipse_prob_lines,
							result_intersections);

	//goalPostDetection(img_posts_binary, goalRoots, hor_hist, ver_hist,
	//				  goalPosts);

	return;
}
