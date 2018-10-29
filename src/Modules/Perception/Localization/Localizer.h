#ifndef LOCALIZER_H_
#define LOCALIZER_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"
#include "Core/Utils/CameraFrame.h"

using namespace std;

class Localizer : public InnerModule
{
    public:
        Localizer(SpellBook *spellBook);
        void Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage);
};

#endif
