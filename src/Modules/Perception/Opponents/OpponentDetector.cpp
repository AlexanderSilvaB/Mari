#include "OpponentDetector.h"

#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

Opponents::Opponents(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
       
}

void Opponents::Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage)
{
    spellBook->perception.vision.BGR = true;
    spellBook->perception.vision.HSV = true;

    //SensorValues sensors;

}
