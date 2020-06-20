#include <math.h>
//#include <cv.h>
//#include <highgui.h>
#include "geometry_utils.h"

//using namespace cv;

double radians(double d)
{
	return d * CV_PI / 180;
}

double degrees(double r)
{
	return r * 180/ CV_PI;
}

bool line_equality(cv::Vec4i line1, cv::Vec4i line2)
{
	bool isSame = true;
	for (int i = 0; i < 4; ++i)
	{
		if(line1[i] != line2[i]) return false;
	}
	return isSame;
}

cv::Point line_middle_point(cv::Vec4i line)
{
	double x1 = line[0], x2 = line[2];
	double y1 = line[1], y2 = line[3];
	return cv::Point(floor((x1+x2) / 2), floor((y1+y2) / 2));
}

bool intersection_in_line(cv::Point point, cv::Vec4i line)
{
	double x1 = line[0], x2 = line[2];
	double y1 = line[1], y2 = line[3];
	if ( point.x < min(x1, x2) || point.x > max(x1, x2) ) return false;
	if ( point.y < min(y1, y2) || point.y > max(y1, y2) ) return false;
	return true;
}

cv::Point* intersection(cv::Vec4i line1, cv::Vec4i line2, cv::Mat image)
{
	double x1 = line1[0], x2 = line1[2];
	double y1 = line1[1], y2 = line1[3];
	double x3 = line2[0], x4 = line2[2];
	double y3 = line2[1], y4 = line2[3];
	double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
	if (d == 0) return NULL;

	double pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
	double x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	double y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

	if(x < 0 || x >= image.rows) return NULL;
	if(y < 0 || y >= image.cols) return NULL;

	cv::Point* ret = new cv::Point();
	ret->x = floor(x);
	ret->y = floor(y);
	return ret;
}

cv::Point* intersection_full(cv::Vec4i line1, cv::Vec4i line2)
{
	double x1 = line1[0], x2 = line1[2];
	double y1 = line1[1], y2 = line1[3];
	double x3 = line2[0], x4 = line2[2];
	double y3 = line2[1], y4 = line2[3];
	double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

	if (d == 0) return NULL;

	double pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
	double x = ( pre * (x3 - x4) - (x1 - x2) * post ) / d;
	double y = ( pre * (y3 - y4) - (y1 - y2) * post ) / d;

	cv::Point* ret = new cv::Point();
	ret->x = floor(x);
	ret->y = floor(y);
	return ret;
}

void line_error(vector<cv::Point> line, cv::Point start, cv::Point best_candidate, double &error)
{
	error = 0;
	for(int i = 0; i < line.size(); i++)
	{
		error += pow(point_line_distance(line[i],
		                                 cv::Vec4i(start.x, start.y, best_candidate.x, best_candidate.y)),3);
	}
	return;
}
void line_error(vector<cv::Point> line1, vector<cv::Point> line2, cv::Point start, cv::Point end, double &error)
{
	error = 0;
	for(int i = 0; i < line1.size(); i++)
	{
		error += pow(point_line_distance(line1[i],
		                                 cv::Vec4i(start.x, start.y, end.x, end.y)),3);
	}
	for(int i = 0; i < line2.size(); i++)
	{
		error += pow(point_line_distance(line2[i],
		                                 cv::Vec4i(start.x, start.y, end.x, end.y)),3);
	}
	return;
}
double points_distance(cv::Point point1, cv::Point point2)
{
	double dx = point2.x - point1.x;
	double dy = point2.y - point1.y;
	double distance = sqrt(dx*dx + dy*dy);
	return distance;
}
double line_length(cv::Vec4i line)
{
	double dx = line[2] - line[0];
	double dy = line[3] - line[1];
	double distance = sqrt(dx*dx + dy*dy);
	return distance;
}
double point_line_distance(cv::Point point, cv::Vec4i line)
{
	double normalLength = hypot(line[2] - line[0], line[3] - line[1]);
	return abs((point.x - line[0]) * (line[3] - line[1]) - (point.y - line[1]) * (line[2] - line[0])) / normalLength;
}
bool equal_points(cv::Point point1, cv::Point point2)
{
	return (point1.x == point2.x && point1.y == point2.y);
}
double points_angle(cv::Point point1, cv::Point point2)
{
	double angle = atan2(point2.x-point1.x,point2.y-point1.y);
	angle = angle * (180 / CV_PI);
	if( angle < 0 )
		angle += 180;
	return angle;
}

double points_angle_360(cv::Point point1, cv::Point point2)
{
	double angle = atan2(point2.x-point1.x,point2.y-point1.y);
	angle = angle * (180 / CV_PI);
	if(angle < 0)
	{
		angle = 360 + angle;
	}
	return angle;
}

double line_angle(cv::Vec4i line)
{
	double angle = atan2(line[2]-line[0],line[3]-line[1]);
	angle = angle * (180 / CV_PI);
	if( angle < 0 )
		angle += 180;
	return angle;
}

cv::Point closest_end_point(cv::Point* inters, cv::Vec4i line)
{
	cv::Point close;
	double temp;
	double min_distance = DBL_MAX;
	for(int p=0; p<2; p++)
	{
		double temp = points_distance(cv::Point(line[2*p], line[2*p+1]), cv::Point(inters->x, inters->y));
		if(temp < min_distance)
		{
			min_distance = temp;
			close = cv::Point(line[2*p], line[2*p+1]);
		}
	}
	return close;
}

cv::Point closest_point(cv::Point* inters, cv::Vec4i line)
{
	cv::Point close;
	if(intersection_in_line(cv::Point(inters->x, inters->y), line))
	{
		return cv::Point(inters->x, inters->y);
	}
	else
	{
		double temp;
		double min_distance = DBL_MAX;
		for(int p=0; p<2; p++)
		{
			double temp = points_distance(cv::Point(line[2*p], line[2*p+1]), cv::Point(inters->x, inters->y));
			if(temp < min_distance)
			{
				min_distance = temp;
				close = cv::Point(line[2*p], line[2*p+1]);
			}
		}
	}
	return close;
}
