#include "VisionModule.h"
#include "Core/Utils/RobotDefs.h"

using namespace cv;

VisionModule::VisionModule(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    ballDetector = new BallDetector(spellBook);
    localizer = new Localizer(spellBook);
    capture = new CombinedCamera();
    top.Update(CAM_W, CAM_H);
    bottom.Update(CAM_W, CAM_H);
}

VisionModule::~VisionModule()
{
    delete ballDetector;
    delete localizer;
}

void VisionModule::Tick(float ellapsedTime)
{
    top.ReadFromYUV422(capture->getFrameTop(), spellBook->perception.vision.BGR, spellBook->perception.vision.HSV, spellBook->perception.vision.GRAY);
    bottom.ReadFromYUV422(capture->getFrameBottom(), spellBook->perception.vision.BGR, spellBook->perception.vision.HSV, spellBook->perception.vision.GRAY);

    if(spellBook->perception.vision.ball.Enabled)
        ballDetector->Tick(ellapsedTime, top, bottom);
    if(spellBook->perception.vision.localization.Enabled)
        localizer->Tick(ellapsedTime, top, bottom);
}