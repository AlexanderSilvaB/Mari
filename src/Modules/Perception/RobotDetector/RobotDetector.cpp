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
    colorsTxt = new int[256 * 256 * 256];
    for (int i = 0; i < 256 * 256 * 256; i++)
    {
        colorsTxt[i] = -1;
    }
    Load("/home/nao/data/vision/opponents.txt");
    buffer = 0;
    equal = 0;
}

void RobotDetector::Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage)
{
    spellBook->perception.vision.BGR = true;
    spellBook->perception.vision.HSV = true;

    if (sonarFilter.sonarFiltered[0].size() != 0)//Reference for SonarStopped
        buffer = sonarFilter.sonarFiltered[0][0];

    Blackboard *blackboard = InitManager::GetBlackboard();
    //SensorValues sensor = readFrom(motion, sensors);
    sonarFilter.update(readFrom(motion, sonarWindow));

    if (sonarFilter.sonarFiltered[0].size() != 0)
    {
        if (buffer == sonarFilter.sonarFiltered[0][0])
            equal++;
        else
            equal = 0;
    }
    if (equal > 20)//Number of repetitive readings of sonar
    {
        spellBook->perception.vision.robotDetector.SonarStopped = true;
    }
    else
    {
        spellBook->perception.vision.robotDetector.SonarStopped = false; 
    }
        

    if (IsObstacle())
    {
        //cout << "Obstacle detected" << endl;
        //obstacle = bottom.BGR;
        //cout << "Bottom: " <<Clustering(obstacle) << endl;
        //obstacle = top.BGR;
        //cout << "Top: " <<Clustering(obstacle) << endl;
    }
    // int menor = 2000;
    // for (int i = 0; i < sonarFilter.sonarFiltered[1].size(); i++)
    // {
    //     if (sonarFilter.sonarFiltered[1][i] < menor)
    //         menor = sonarFilter.sonarFiltered[1][i];
    // }
    // cout << menor << endl;
    // if (menor < 450)
    // {
    //     spellBook->perception.vision.robotDetector.middleDetected = true;
    // }

    // else
    // {
    //     spellBook->perception.vision.robotDetector.middleDetected = false;
    // }
}

bool RobotDetector::IsObstacle()
{
    int distance = 1000; //mm

    for (int i = 0; i < sonarFilter.sonarFiltered.size(); i++)
        for (int j = 0; j < sonarFilter.sonarFiltered[i].size(); j++)
        {
            if (i == LEFT)
            {
                if (sonarFilter.sonarFiltered[LEFT][j] < distance)
                {
                    spellBook->perception.vision.robotDetector.leftDetected = true;
                    //cout << "LEFT DETECTED" << endl;
                    return true;
                }
                else
                    spellBook->perception.vision.robotDetector.leftDetected = false;
            }
            else if (i == MIDDLE)
            {
                //cout << sonarFilter.sonarFiltered[MIDDLE][j] << endl;
                if (sonarFilter.sonarFiltered[MIDDLE][j] < distance)
                {
                    spellBook->perception.vision.robotDetector.middleDetected = true;
                    //cout << "MIDDLE DETECTED" << endl;
                    return true;
                }
                else
                    spellBook->perception.vision.robotDetector.middleDetected = false;
            }
            else if (i == RIGHT)
            {
                if (sonarFilter.sonarFiltered[RIGHT][j] < distance)
                {
                    spellBook->perception.vision.robotDetector.rightDetected = true;
                    //cout << "RIGHT DETECTED" << endl;
                    return true;
                }
                else
                    spellBook->perception.vision.robotDetector.rightDetected = false;
            }
        }
    return false;
}

int RobotDetector::Clustering(cv::Mat img)
{
    int red, green, blue;
    int col;
    int counter = 0;

    for (int y = 0; y < img.rows; y++)
    {
        for (int x = 0; x < img.cols; x++)
        {
            cv::Vec3b color = img.at<cv::Vec3b>(y, x);
            blue = (int)color.val[0];
            green = (int)color.val[1];
            red = (int)color.val[2];
            col = (blue << 16) | (green << 8) | red;

            if (colorsTxt[col] != -1)
            {
                color.val[0] = 0;
                color.val[1] = 0;
                color.val[2] = 255;
                counter++;
            }
            img.at<cv::Vec3b>(y, x) = color;
        }
    }
    return counter;
}

void RobotDetector::Load(std::string file)
{
    int color;
    ifstream inFile(file.c_str(), ios::in);
    if (inFile)
    {
        while (inFile >> color)
        {
            colorsTxt[color] = RED;
        }
        inFile.close();
    }
    else
        cout << "could not open file" << endl;
}