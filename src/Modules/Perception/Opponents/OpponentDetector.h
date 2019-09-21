#ifndef OPPONENT_H_
#define OPPONENT_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"
#include "Core/Utils/CameraFrame.h"
#include "Modules/Motion/rUNSWiftMotionAdapter.h"

using namespace std;

class Opponents : public InnerModule
{
    public:
        Opponents(SpellBook *spellBook);
        void Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage);
    //private:
        SonarRecorder sonarRecorder;
};

#endif
