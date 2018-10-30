#ifndef _Vision_H_
#define _Vision_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"
#include <opencv2/opencv.hpp>
#include "Core/Utils/CameraFrame.h"

#include "Core/Utils/CombinedCamera.hpp"
#include "Ball/BallDetector.h"
#include "Localization/Localizer.h"

#include "Core/Utils/FrameWriter.h"

using namespace std;

class VisionModule : public InnerModule
{
    private:
        CombinedCamera *capture;

        CameraFrame top;
        CameraFrame bottom;

        FrameWriter *frameWriter;

        BallDetector *ballDetector;
        Localizer *localizer;

        cv::Mat combinedImage;

    public:
        VisionModule(SpellBook *spellBook);
        ~VisionModule();
        void Tick(float ellapsedTime);
};

#endif