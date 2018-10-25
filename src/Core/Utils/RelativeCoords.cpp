#include "RelativeCoords.h"
#include "RobotDefs.h"
#include <cmath>
#include <iostream>

using namespace std;

RelativeCoords::RelativeCoords()
{
    this->distance = 0;
    this->yaw = 0;
    this->pitch = 0;
}

RelativeCoords::RelativeCoords(const RelativeCoords &coord)
{
    this->distance = coord.distance;
    this->yaw = coord.yaw;
    this->pitch = coord.pitch;
}

RelativeCoords::RelativeCoords(float distance, float yaw, float pitch)
{
    this->distance = distance;
    this->yaw = yaw;
    this->pitch = pitch;
}

float RelativeCoords::getDistance()
{
    return distance;
}

float RelativeCoords::getYaw()
{
    return yaw;
}

float RelativeCoords::getPitch()
{
    return pitch;
}

void RelativeCoords::setDistance(float distance)
{
    this->distance = distance;
}

void RelativeCoords::setYaw(float yaw)
{
    this->yaw = yaw;
}

void RelativeCoords::setPitch(float pitch)
{
    this->pitch = pitch;
}

void RelativeCoords::fromPixel(int w, int h, float headYaw, float headPitch, bool upper)
{
    yaw = ((w / (float)CAM_W) - 0.5f) * H_FOV;
    pitch = -((h / (float)CAM_H) - 0.5f) * V_FOV;
    if(!upper)
        pitch += CAM_OFSET_ANGLE;

    yaw = yaw - headYaw;
    pitch = pitch + headPitch;

    float y = ROBOT_HEIGHT / tan(-pitch);
    float x = y*tan(-yaw);
    distance = sqrt(x*x + y*y);
}