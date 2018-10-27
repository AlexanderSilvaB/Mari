#ifndef _VisionModule_H_
#define _VisionModule_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"
#include <opencv2/opencv.hpp>
#include "Core/Utils/CameraFrame.h"

#include "perception/vision/camera/CombinedCamera.hpp"
#include "Ball/BallDetector.h"
#include "Localization/Localizer.h"

using namespace std;

class VisionModule : public InnerModule
{
    private:
        CombinedCamera *capture;

        CameraFrame top;
        CameraFrame bottom;

        BallDetector *ballDetector;
        Localizer *localizer;

    public:
        VisionModule(SpellBook *spellBook);
        ~VisionModule();
        void Tick(float ellapsedTime);
};

#endif