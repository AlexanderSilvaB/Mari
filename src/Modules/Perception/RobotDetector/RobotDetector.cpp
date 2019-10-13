#include "RobotDetector.h"

#include "Core/InitManager.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CombinedCamera.hpp"

#include "perception/vision/CameraToRR.hpp"
#include "perception/vision/WhichCamera.hpp"
#include <iomanip>
#include <iostream>
#include <fstream>

RobotDetector::RobotDetector(SpellBook *spellBook)
    : InnerModule(spellBook)
{
}

void RobotDetector::Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage)
{
    spellBook->perception.vision.BGR = true;
    spellBook->perception.vision.HSV = true;

    Blackboard *blackboard = InitManager::GetBlackboard();
    SensorValues sensor = readFrom(motion, sensors);

    filter.update(readFrom(motion, sonarWindow));
    //writeTo(kinematics, sonarFiltered, filter.sonarFiltered);

    //cout << filter.sonarFiltered.at(1) << endl;

    for (int i = 0; i < filter.sonarFiltered.size(); i++)
    {
        for (int j = 0; j < filter.sonarFiltered[i].size(); j++)
            cout << filter.sonarFiltered[i][j] << " ";
        cout << endl;
    }

    for (int i = 0; i < 10; i++)
    {
        //cout << sensor.sonar[i] << "  ";
    }

    //cout << endl << endl << endl;

    for (int i = 9; i < 20; i++)
    {
        //cout << sensor.sonar[i] << "  ";
    }

    //cout << endl << endl << endl;
}
