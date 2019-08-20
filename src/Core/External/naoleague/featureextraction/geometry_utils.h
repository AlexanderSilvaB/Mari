#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <math.h>
#include <opencv2/opencv.hpp>
#include <vector>


//#include <cv.h>
//#include <highgui.h>

//using namespace cv;
using namespace std;

cv::Point line_middle_point(cv::Vec4i line);

bool line_equality(cv::Vec4i line1, cv::Vec4i line2);

bool intersection_in_line(cv::Point point, cv::Vec4i line);

void line_error(std::vector<cv::Point> line, cv::Point start, cv::Point best_candidate, double &error);

void line_error(std::vector<cv::Point> line1, std::vector<cv::Point> line2, cv::Point start, cv::Point end, double &error);

double points_distance(cv::Point point1, cv::Point point2);

double point_line_distance(cv::Point point, cv::Vec4i line);

bool equal_points(cv::Point point1, cv::Point point2);

double points_angle(cv::Point point1, cv::Point point2);

cv::Point* intersection(cv::Vec4i line1, cv::Vec4i line2, cv::Mat image);

cv::Point* intersection_full(cv::Vec4i line1, cv::Vec4i line2);

double line_angle(cv::Vec4i line);

double points_angle_360(cv::Point point1, cv::Point point2);

cv::Point closest_end_point(cv::Point* inters, cv::Vec4i line);

cv::Point closest_point(cv::Point* inters, cv::Vec4i line);

double line_length(cv::Vec4i line);

double radians(double d);

double degrees(double r);

#endif
