#include "Core/Module.h"
#include "Core/Utils/Circle.h"
#include <opencv2/opencv.hpp>
#include <vector>

#ifdef USE_V4L2
#include "perception/vision/camera/CombinedCamera.hpp"
#else
#ifdef USE_QIBUILD
#include <RinoLib/Perception/Capture.h>
using namespace Rinobot::Nao;
using namespace Rinobot::Nao::Perception;
#endif
#endif
#if USE_UNSW
#include "perception/vision/camera/CameraToRR.hpp"
#endif

using namespace std;

class BallDetector : public Module
{
    public:
        enum Methods { CASCADE, GEOMETRIC, NEURAL };
    private:
        #ifdef USE_V4L2
        CombinedCamera *capture;
        #else
        #ifdef USE_QIBUILD
        Capture *capture;
        #else
        cv::VideoCapture capture;
        #endif
        #endif

        #if USE_UNSW
        CameraToRR conv_rr_;
        #endif
        float closeInPitch, farPitch, ballYaw, ballPitch;
        bool ballCloseIn;
        float ballCloseInDistance, targetYaw, targetPitch;
        int ballLostCount;
        XYZ_Coord neckRelative;

        cv::CascadeClassifier cascade;
        cv::Mat img;
        Circle ball;
        float w_, h_, x_, y_, yc, alpha;
        float angles[2];

        Methods method;

        bool CascadeMethod();
        bool GeometricMethod();
        bool NeuralMethod();
        float CalculateDesiredPitch(XYZ_Coord &neckRelativeTarget);
        float CalculateDesiredYaw(XYZ_Coord &neckRelativeTarget);
    public:
        BallDetector(SpellBook *spellBook);
        void Tick(float ellapsedTime);
};