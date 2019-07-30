#include "Core/Utils/Math.h"
#include "FeatureExtractor.h"
#include "Core/InitManager.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CombinedCamera.hpp"

#include "perception/vision/CameraToRR.hpp"
#include "perception/vision/WhichCamera.hpp"

#define R 2.0f
#define PI_2 1.5707963267f
#define TICK_TIME 30

using namespace cv;

FeatureExtractor::FeatureExtractor(SpellBook *spellBook)
    : InnerModule(spellBook)
{
    targetYaw = 0;
    targetPitch = 0;
}

void FeatureExtractor::Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage)
{
    spellBook->perception.vision.BGR = true;
    spellBook->perception.vision.HSV = true;
    spellBook->perception.vision.GRAY = true;
    Blackboard *blackboard = InitManager::GetBlackboard();
    SensorValues sensor = readFrom(motion, sensors);

    img_rgb = bottom.BGR;
    extract_features(img_rgb, result_intersections, goalPosts);

    bool detected = false;
    int Ldetected = 0;
    int featureIndex = 0;
    if (result_intersections.size() != 0)
        detected = true;
    for (int i = 0; i < result_intersections.size(); i++)
    {
        if (result_intersections[i].type == L)
        {
            detected = true;
            Ldetected++;
            featureIndex = i;
            break;
        }
    }
    cout << "teste 1" << endl;
    if(spellBook != NULL)
        cout << "spellBook: OKK" << endl;
    else
    {
        cout << "spellBook: Invalid" << endl;
        return;
    }
    if(result_intersections.size() > featureIndex)
        cout << "featureIndex: OK" << endl;
    else if(result_intersections.size() != 0)
        cout << "result intersections" << endl;
    else
    {
        cout << "featureIndex: Invalidd" << endl;
        //return;
    }
    //spellBook->perception.vision.feature.ImageX = result_intersections[featureIndex].position.x;
    //spellBook->perception.vision.feature.ImageY = result_intersections[featureIndex].position.y;
    //spellBook->perception.vision.feature.FeatureDetected = detected;
    cout << "teste 2" << endl;

    if (detected)
    {
        // cout << "teste 3" << endl;
        // spellBook->perception.vision.feature.FeatureLostCount = 0;
        // spellBook->perception.vision.feature.TimeSinceFeatureSeen = 0;
        // cout << "teste 4" << endl;
        // float currHeadYaw = sensor.joints.angles[Joints::HeadYaw];
        // float currHeadPitch = sensor.joints.angles[Joints::HeadPitch];
        // cout << "teste 5" << endl;
        // RelativeCoord rr;
        // rr.fromPixel(result_intersections[featureIndex].position.x, result_intersections[featureIndex].position.y, currHeadYaw, currHeadPitch);
        // spellBook->perception.vision.feature.FeatureYaw = rr.getYaw();
        // spellBook->perception.vision.feature.FeatureDistance = rr.getDistance();
        // spellBook->perception.vision.feature.FeaturePitch = rr.getPitch();
        // cout << "teste 6" << endl;
        // float CONSTANT_X = (float)CAM_FEATURE_W / H_DOF;
        // float xDiff = -(result_intersections[featureIndex].position.x - (CAM_FEATURE_W / 2)) / CONSTANT_X;
        // spellBook->perception.vision.feature.HeadYaw = xDiff - currHeadYaw;
        // cout << "teste 7" << endl;
        // float CONSTANT_Y = (float)CAM_FEATURE_H / V_DOF;
        // float yDiff = (result_intersections[featureIndex].position.y - (CAM_FEATURE_H / 2)) / CONSTANT_Y;
        // spellBook->perception.vision.feature.HeadPitch = yDiff - currHeadPitch;

        //cout << rr.getDistance() << "m, " << Rad2Deg(rr.getYaw()) << "º" << endl;
    }
    else
    {
        cout << "teste 8" << endl;

        //spellBook->perception.vision.feature.FeatureLostCount++;
        //spellBook->perception.vision.feature.TimeSinceFeatureSeen += ellapsedTime;
        cout << "teste 9" << endl;

    }
        cout << "teste 10" << endl;

}

//cv::RNG rng(12345);