#ifndef BALL_DETECTOR_H_
#define BALL_DETECTOR_H_

#include "Core/Module.h"
#include "Core/Utils/Circle.h"
#include <opencv2/opencv.hpp>
#include <vector>

#include "Core/SpellBook.h"

using namespace std;

class BallDetector : public InnerModule
{
    public:
        enum Methods { CASCADE, GEOMETRIC, NEURAL };
    private:
        float targetYaw, targetPitch;
        float distance;
        float speed;

        cv::CascadeClassifier cascade;
        cv::Mat img;
        Circle ball;

        Methods method;

        bool CascadeMethod();
        bool GeometricMethod();
        bool NeuralMethod();
    public:
        BallDetector(SpellBook *spellBook);
        void Tick(float ellapsedTime, cv::Mat &img);
};

#endif
