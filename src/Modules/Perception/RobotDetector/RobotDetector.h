#ifndef ROBOT_DETECTOR_H_
#define ROBOT_DETECTOR_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"
#include "Core/Utils/CameraFrame.h"
#include "Core/External/unsw/unsw/perception/kinematics/SonarFilter.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

using namespace std;

#define LEFT 0
#define MIDDLE 1
#define RIGHT 2

#define RED ((0 << 16) | (0 << 8) | 255)

class RobotDetector : public InnerModule
{
    private: 
        cv::Mat obstacle;
        SonarFilter sonarFilter;
        int *colorsTxt;
    public:
        RobotDetector(SpellBook *spellBook);
        void Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage);
        void Load(std::string file);
        bool IsObstacle();
        int Clustering(cv::Mat img); //Returns the number of pixels painted // > 7000 possible a robot
        
};

#endif
