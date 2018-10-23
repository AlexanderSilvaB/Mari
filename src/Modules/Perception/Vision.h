#ifndef _Vision_H_
#define _Vision_H_

#include "Core/SpellBook.h"
#include <opencv2/opencv.hpp>
#include "Core/Utils/CameraFrame.h"

#include "Core/Utils/CombinedCamera.hpp"
#include "Ball/BallDetector.h"
#include "Localization/Localizer.h"

using namespace std;

class Vision
{
    private:
        SpellBook *spellBook;
        CombinedCamera *capture;

        CameraFrame frame;
        cv::Mat img;

        BallDetector *ballDetector;
        Localizer *localizer;

    public:
        Vision(SpellBook *spellBook);
        ~Vision();
        void Tick(float ellapsedTime);
};

#endif