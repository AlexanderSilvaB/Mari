#ifndef BALL_DETECTOR_H_
#define BALL_DETECTOR_H_

#include "Core/Module.h"
#include "Core/Utils/Circle.h"
#include <opencv2/opencv.hpp>
#include <vector>
#include "Core/Utils/CameraFrame.h"

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
        Circle ball;

        Methods method;

        bool CascadeMethod(CameraFrame &top, CameraFrame &bottom);
        bool GeometricMethod(CameraFrame &top, CameraFrame &bottom);
        bool NeuralMethod(CameraFrame &top, CameraFrame &bottom);

        int *colorsTxt;
    public:
        BallDetector(SpellBook *spellBook);
        void Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage);
        void Load(std::string file);
        void Clustering(cv::Mat img);
};

#endif
