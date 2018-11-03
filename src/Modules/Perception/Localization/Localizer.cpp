#include "Localizer.h"

#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

Localizer::Localizer(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
       
}

void Localizer::Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage)
{
    spellBook->perception.vision.BGR = true;
    spellBook->perception.vision.HSV = true;
}
