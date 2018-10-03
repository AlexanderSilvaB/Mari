#ifndef _Vision_H_
#define _Vision_H_

#include "Core/SpellBook.h"
#include <opencv2/opencv.hpp>
#include "Core/Utils/CameraFrame.h"

#ifdef USE_V4L2
#include "Core/Utils/CombinedCamera.hpp"
#else
#ifdef USE_QIBUILD
#include <RinoLib/Perception/Capture.h>
using namespace Rinobot::Nao;
using namespace Rinobot::Nao::Perception;
#endif
#endif

#include "Ball/BallDetector.h"

using namespace std;

class Vision
{
    private:
        SpellBook *spellbook;

        #ifdef USE_V4L2
        CombinedCamera *capture;
        #else
        #ifdef USE_QIBUILD
        Capture *capture;
        #else
        cv::VideoCapture capture;
        #endif
        #endif

        CameraFrame frame;
        cv::Mat img;

        BallDetector *ballDetector;

    public:
        Vision(SpellBook *spellBook);
        ~Vision();
        void Tick(float ellapsedTime);
};

#endif