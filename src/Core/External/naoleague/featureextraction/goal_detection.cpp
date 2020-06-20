#include <iostream>
#include <math.h>
#include <time.h>
//#include <cv.h>
//#include <highgui.h>
#include "geometry_utils.h"
#include "goal_detection.h"
#include "line_detection.h"
#include "hough_line_detection.h"

using namespace std;
//using namespace cv;

double unifRand()
{
	return rand() / double(RAND_MAX);
}
double unifRand(double a, double b)
{
	return (b-a)*unifRand() + a;
}

void seed()
{
	srand(time(0));
}
int horizontal_post(cv::Mat image, vector<cv::Vec4i> lines_hor, vector<posts_lines> best_candidate_lines, cv::Vec4i &result)
{
	if(best_candidate_lines.size() == 0)
	{
		double best_measure = DBL_MAX;
		int best_hor_match = 0;
		for (int i = 0; i < lines_hor.size(); ++i)
		{
			double hor_line_angle = line_angle(lines_hor[i]);
			double hor_angle_measure = 1 - abs(hor_line_angle - 90) / 90;
			double length_measure = 1 - line_length(lines_hor[i]) / image.cols;
			double temp_sum_measure = length_measure * hor_angle_measure;
			if(temp_sum_measure < best_measure)
			{
				best_measure = temp_sum_measure;
				best_hor_match = i;
			}
		}
		result = lines_hor[best_hor_match];
		return 0;
	}
	else
	{
		double best_measure = DBL_MAX;
		int best_hor_match = 0;
		for (int i = 0; i < lines_hor.size(); ++i)
		{
			double hor_line_angle = line_angle(lines_hor[i]);
			double hor_angle_measure = 1 - abs(hor_line_angle - 90) / 90;
			double crossing_measure = 0;
			cv::Point middle_point = line_middle_point(lines_hor[i]);
			double length_measure = 1 - line_length(lines_hor[i]) / image.cols;
			double position_measure = 0;
			for (int j = 0; j < best_candidate_lines.size(); ++j)
			{
				cv::Point* intersect = intersection_full(lines_hor[i], best_candidate_lines[j].line);
				if (intersect != NULL)
				{
					if(best_candidate_lines[j].line[0] < best_candidate_lines[j].line[2])
					{
						crossing_measure += points_distance(cv::Point(best_candidate_lines[j].line[0],
						                                    best_candidate_lines[j].line[1]), cv::Point(intersect->x, intersect->y));
						position_measure += abs(middle_point.x - best_candidate_lines[j].line[0]);
					}
					else
					{
						crossing_measure += points_distance(cv::Point(best_candidate_lines[j].line[2],
						                                    best_candidate_lines[j].line[3]), cv::Point(intersect->x, intersect->y));
						position_measure += abs(middle_point.x - best_candidate_lines[j].line[2]);
					}
				}
				else
				{
					crossing_measure = DBL_MAX;
				}
			}
			double temp_sum_measure = length_measure * (50 * hor_angle_measure + pow(position_measure,2) + pow(crossing_measure,2));
			if(temp_sum_measure < best_measure)
			{
				best_measure = temp_sum_measure;
				best_hor_match = i;
			}
		}
		result = lines_hor[best_hor_match];
		if (best_measure < 150) return 1;
		return -1;
	}
}

void vertical_posts(cv::Mat image, int x_offset, vector<cv::Vec4i> lines_ver, vector<int> candidate_cols, vector<posts_lines> &best_candidate_lines)
{
	double match_measure;
	for (int i = 0; i < candidate_cols.size(); ++i)
	{
		int best_match = 0;
		double match_measure = DBL_MAX;
		double ver_line_angle;
		double angle_measure;
		double position_measure;
		double temp_match_measure;
		double length_measure;
		cv::Point middle_point;
		for(int j = 0; j < lines_ver.size(); j ++)
		{
			ver_line_angle = line_angle(lines_ver[j]);
			angle_measure = abs(ver_line_angle - 90);
			middle_point = line_middle_point(lines_ver[j]);
			position_measure = pow(abs(middle_point.y + x_offset - candidate_cols[i]),2);
			length_measure = (1 - line_length(lines_ver[j]) / image.rows);
			temp_match_measure = length_measure * (angle_measure + position_measure/2);
			if (temp_match_measure < match_measure)
			{
				match_measure = temp_match_measure;
				best_match = j;
			}
		}
		if(match_measure < 50)
		{
			posts_lines temp;
			temp.candidate = i;
			temp.line = lines_ver[best_match];
			best_candidate_lines.push_back(temp);
			lines_ver.erase(lines_ver.begin() + best_match);
		}
	}
}

void extend_line(cv::Mat image, cv::Vec4i &line)
{
	cv::Point left, right;
	left = cv::Point(line[0], line[1]);
	right = cv::Point(line[2], line[3]);
	double angle_top_bottom = points_angle_360(left, right);
	bool end = false;
	cv::Point newPoint;
	int len = 0;
	do
	{
		newPoint.x = floor(right.x + len * sin(CV_PI * angle_top_bottom/180));
		newPoint.y = floor(right.y + len * cos(CV_PI * angle_top_bottom/180));
		if(newPoint.x < 0 || newPoint.y < 0 || newPoint.x >= image.rows || newPoint.y >= image.cols ||
		        (int)image.at<cv::Vec3b>(newPoint.x,newPoint.y)[0] == 0)
		{
			end = true;
		}
		else
		{
			line[0] = newPoint.x;
			line[1] = newPoint.y;
		}
		len++;
	}
	while(!end);

	end = false;
	len = 0;
	angle_top_bottom = points_angle_360(right, left);
	do
	{
		newPoint.x = floor(left.x + len * sin(CV_PI * angle_top_bottom/180));
		newPoint.y = floor(left.y + len * cos(CV_PI * angle_top_bottom/180));
		if(newPoint.x < 0 || newPoint.y < 0 || newPoint.x >= image.rows || newPoint.y >= image.cols ||
		        (int)image.at<cv::Vec3b>(newPoint.x,newPoint.y)[0] == 0)
		{
			end = true;
		}
		else
		{
			line[2] = newPoint.x;
			line[3] = newPoint.y;
		}
		len++;
	}
	while(!end);
}

cv::Rect crop_region_interest(cv::Mat image, double* hor_hist, int* ver_hist, vector<int> local_maxima)
{
	if(local_maxima.size() == 0)
	{
		return cv::Rect(0,0,image.cols, image.rows);
	}
	int right_threshold = 0;
	bool right = false;
	bool left = false;
	int left_thershold = 0;
	int top_interest = 0;
	int bottom_interest = 0;
	//horizontal area of interest
	for (int i = 0; i < image.rows; i++)
	{
		if(ver_hist[i] > 0.0)
		{
			top_interest = i;
			break;
		}
	}
	for (int i = image.rows; i >= 0; i--)
	{
		if(ver_hist[i] > 0.0)
		{
			bottom_interest = i;
			break;
		}
	}
	// vertical area of interest...
	int counter = 0;
	int candidate = 0;
	for( int i=local_maxima[candidate]; i >= 0; i--)
	{
		if(hor_hist[i] == 0)
		{
			counter ++;
			if(counter == CROP_THRESHOLD)
			{
				left_thershold = i;
				left = true;
				break;
			}
		}
		else
		{
			counter = 0;
		}
	}
	counter = 0;
	candidate = local_maxima.size() - 1;
	for( int i=local_maxima[candidate]; i < image.cols; i++)
	{
		if(hor_hist[i] == 0)
		{
			counter ++;
			if(counter == CROP_THRESHOLD)
			{
				right_threshold = i;
				right = true;
				break;
			}
		}
		else
		{
			counter = 0;
		}
	}
	int x = (left) ? (left_thershold) : 0;
	int y = top_interest;
	int width = (right) ? (right_threshold - x) : (image.cols - x);
	int height = bottom_interest - top_interest;
	return cv::Rect(x,y,width,height);
}

int measure_width(cv::Mat image, cv::Vec4i line, cv::Point point)
{
	cv::Point bottom = (line[0] > line[2]) ? cv::Point(line[0], line[1]): cv::Point(line[2], line[3]);
	cv::Point middle_point = line_middle_point(line);
	double angle_left = points_angle_360(middle_point, bottom) + 90;
	double angle_right = points_angle_360(middle_point, bottom) - 90;
	bool end = false;
	int counter_right = 0;
	int len = 0;
	cv::Point newPoint = point;
	do
	{
		newPoint.x = floor(newPoint.x + len * sin(CV_PI * angle_right/180));
		newPoint.y = floor(newPoint.y + len * cos(CV_PI * angle_right/180));
		if(newPoint.x < 0 || newPoint.y < 0 || newPoint.x >= image.rows || newPoint.y >= image.cols ||
		        (int)image.at<cv::Vec3b>(newPoint.x,newPoint.y)[0] == 0)
		{
			end = true;
		}
		else
		{
			counter_right ++;
		}
		len++;
	}
	while(!end);
	end = false;
	int counter_left = 0;
	len = 0;
	newPoint = point;
	do
	{
		newPoint.x = floor(newPoint.x + len * sin(CV_PI * angle_left/180));
		newPoint.y = floor(newPoint.y + len * cos(CV_PI * angle_left/180));
		if(newPoint.x < 0 || newPoint.y < 0 || newPoint.x >= image.rows || newPoint.y >= image.cols ||
		        (int)image.at<cv::Vec3b>(newPoint.x,newPoint.y)[0] == 0)
		{
			end = true;
		}
		else
		{
			counter_left ++;
		}
		len++;
	}
	while(!end);
	return counter_right + counter_left;
}

double average_sampling_width(cv::Mat image, cv::Vec4i line)
{
	cv::Point middle_point = line_middle_point(line);
	cv::Point bottom = (line[0] > line[2]) ? cv::Point(line[0], line[1]): cv::Point(line[2], line[3]);
	double angle_mid_bottom = points_angle_360(middle_point, bottom);
	int sum_width = 0;
	for (int j = 0; j < 20; ++j)
	{
		int len = floor(unifRand(0.0, points_distance(middle_point, bottom)));
		cv::Point newPoint;
		newPoint.x = floor(middle_point.x + len * sin(CV_PI * angle_mid_bottom/180));
		newPoint.y = floor(middle_point.y + len * cos(CV_PI * angle_mid_bottom/180));
		sum_width += measure_width(image, line, newPoint);
	}
	return (double)sum_width / 20.0;
}

double average_sampling_height(cv::Mat image, cv::Vec4i line)
{
	cv::Point middle_point = line_middle_point(line);
	cv::Point left = (line[1] < line[3]) ?
	             cv::Point(line[0], line[1]):
	             cv::Point(line[2], line[3]);
	double angle_mid_bottom = points_angle_360(middle_point, left);
	int sum_width = 0;
	for (int j = 0; j < 20; ++j)
	{
		int len = floor(unifRand(-points_distance(middle_point, left) / 2, points_distance(middle_point, left) / 2));
		cv::Point newPoint;
		newPoint.x = floor(middle_point.x + len * sin(CV_PI * angle_mid_bottom/180));
		newPoint.y = floor(middle_point.y + len * cos(CV_PI * angle_mid_bottom/180));
		sum_width += measure_width(image, line, newPoint);
	}
	return (double)sum_width / 20.0;
}

void post_final(cv::Mat image, int x_offset, vector<goalposts> &goalPosts, vector<cv::Point> goalRoots)
{
	for (int i = 0; i < goalPosts.size(); ++i)
	{
		if (goalPosts[i].type == L_POST || goalPosts[i].type == R_POST || goalPosts[i].type == V_POST)
		{
			double confidence = 0;
			int counter = 0;
			for (int j = 0; j < goalRoots.size(); ++j)
			{
				if(abs(goalRoots[j].x - goalPosts[i].root_position.x) < ROOT_OFFSET_Y){
					counter ++;
					confidence += 1 - abs(goalPosts[i].root_position.y + x_offset - goalRoots[j].y)/image.rows;
				}
			}
			confidence /= (double)counter + 0.1;
			goalPosts[i].root_confidence = confidence;
		}
		if (goalPosts[i].type == V_POST)
		{
			for (int k = 0; k < goalPosts.size(); ++k)
			{
				if (goalPosts[k].type == H_POST)
				{
					cv::Point top = (goalPosts[i].line[0] > goalPosts[i].line[2]) ?
				            cv::Point(goalPosts[i].line[2], goalPosts[i].line[3]):
				            cv::Point(goalPosts[i].line[0], goalPosts[i].line[1]);
				    cv::Point close;
				    cv::Point far;
				    if(points_distance(top, goalPosts[k].root_position) < points_distance(top, goalPosts[k].top_position))
				    {
				    	close = goalPosts[k].root_position;
				    	far = goalPosts[k].top_position;
				    }
				    else
				    {
				    	far = goalPosts[k].root_position;
				    	close = goalPosts[k].top_position;
				    }
				    double angle = points_angle_360(close, far);

				    goalPosts[i].type = (angle > 90 && angle < 270) ? R_POST:L_POST;  
				}
			}
		}
	}
	// fix position in respect to the original image
	for (int i = 0; i < goalPosts.size(); ++i)
	{
		goalPosts[i].root_position.y += x_offset;
		goalPosts[i].top_position.y += x_offset;
		goalPosts[i].line[1] += x_offset;
		goalPosts[i].line[3] +=  x_offset;
	}
	return;
}

void goalPostDetection(cv::Mat image, vector<cv::Point> goalRoots, double* hor_hist, int* ver_hist, vector<goalposts> &goalPosts)
{
	vector<int> candidate_cols;
	bool inTransition = false;
	double last_maximum = 0.0;
	int last_candidate = 0;
	cv::Mat result = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
	cv::Mat hist = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
	cv::Mat temp = cv::Mat::zeros(image.rows, image.cols, CV_8UC3);
	// find local maxima in the histogram...
	for( int i = 0; i < image.cols; i++ )
	{
		if(inTransition)
		{
			if(i == image.cols - 1)
				candidate_cols.push_back(last_candidate);
			if( hor_hist[i] > last_maximum )
			{
				inTransition = true;
				last_maximum = hor_hist[i];
				last_candidate = i;
			}
			else
			{
				if(hor_hist[i] < CONTROL_MAX * last_maximum)
				{
					inTransition = false;
					candidate_cols.push_back(last_candidate);
				}
			}
		}
		else
		{
			if( hor_hist[i] > HIST_THRESHOLD )
			{
				inTransition = true;
				last_maximum = hor_hist[i];
				last_candidate = i;
			}
		}
	}
	if(candidate_cols.size() > 2 ) return;

	cv::Mat cropped;
	image.copyTo(cropped);
	vector<posts_lines> best_candidate_lines;

	// crop the image leaving only the interesting part of it...
	cv::Rect roi = crop_region_interest(image, hor_hist, ver_hist, candidate_cols);
	cropped = cropped(roi);

	if (candidate_cols.size() != 0)
	{
		// find lines sampling the images only for vertical lines..
		vector<cv::Vec4i> lines_ver;
		line_extraction(cropped, lines_ver, SAMPLING_VER, 0);
		// find lines from the produced which present goalposts
		// near local maxima positions...
		if(lines_ver.size() != 0)
			vertical_posts(image, roi.x, lines_ver, candidate_cols, best_candidate_lines);

		// extend these lines until to find black...
		bool isBothVisible = false;
		if(best_candidate_lines.size() == 2) isBothVisible = true;
		for (int i = 0; i < best_candidate_lines.size(); ++i)
		{
			extend_line(cropped, best_candidate_lines[i].line);
			cv::Point bottom = (best_candidate_lines[i].line[0] > best_candidate_lines[i].line[2]) ?
			               cv::Point(best_candidate_lines[i].line[0], best_candidate_lines[i].line[1]):
			               cv::Point(best_candidate_lines[i].line[2], best_candidate_lines[i].line[3]);

			cv::Point top = (best_candidate_lines[i].line[0] > best_candidate_lines[i].line[2]) ?
			            cv::Point(best_candidate_lines[i].line[2], best_candidate_lines[i].line[3]):
			            cv::Point(best_candidate_lines[i].line[0], best_candidate_lines[i].line[1]);

			goalposts temp;
			temp.type = V_POST;
			if(isBothVisible)
			{
				temp.type = (i == 0) ? L_POST : R_POST;
			}
			temp.line = best_candidate_lines[i].line;
			temp.root_position = bottom;
			temp.root_confidence = 0.0;
			temp.top_position = top;
			temp.width = average_sampling_width(cropped, best_candidate_lines[i].line);
			goalPosts.push_back(temp);
		}
		// find lines now only sampling for horizontal lines...
		vector<cv::Vec4i> lines_hor;
		cv::Vec4i line_hor_pass;
		line_extraction(cropped, lines_hor, 0, SAMPLING_HOR);

		// based on the vertical lines find the best line which is the horizontal post
		// returns 2 if the lined found is actual very close to be considered a horizontal post
		// returns 0 if its probably not an horizontal post
		// returns 1 if there were no vertical posts to back our decision, the longest horizontal line
		// is returned...
		// returns -1 if there is no candidate to be vertical post
		if(lines_hor.size() != 0)
		{
			int find_horizontal = horizontal_post(image, lines_hor, best_candidate_lines, line_hor_pass);
			if(find_horizontal == 0 || find_horizontal == 1)
			{
				extend_line(cropped, line_hor_pass);
				goalposts temp;
				temp.type = H_POST;
				temp.line = line_hor_pass;

				cv::Point left = (line_hor_pass[1] < line_hor_pass[3]) ?
				             cv::Point(line_hor_pass[0], line_hor_pass[1]):
				             cv::Point(line_hor_pass[2], line_hor_pass[3]);

				cv::Point right = (line_hor_pass[1] < line_hor_pass[3]) ?
				              cv::Point(line_hor_pass[2], line_hor_pass[3]):
				              cv::Point(line_hor_pass[0], line_hor_pass[1]);

				temp.root_position = left;
				temp.root_confidence = 0.0;
				temp.top_position = right;
				temp.width = average_sampling_height(cropped, line_hor_pass);
				goalPosts.push_back(temp);
			}
		}
		post_final(image, roi.x, goalPosts, goalRoots);
		return;
	}
}
