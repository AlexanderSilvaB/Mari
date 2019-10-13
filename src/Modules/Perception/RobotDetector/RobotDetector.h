#ifndef ROBOT_DETECTOR_H_
#define ROBOT_DETECTOR_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"
#include "Core/Utils/CameraFrame.h"
#include "Core/External/unsw/unsw/perception/kinematics/SonarFilter.hpp"

using namespace std;

class RobotDetector : public InnerModule
{
    public:
        SonarFilter filter;
        RobotDetector(SpellBook *spellBook);
        void Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage);
};

#endif
