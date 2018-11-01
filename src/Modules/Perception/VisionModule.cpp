#include "VisionModule.h"
#include "Core/Utils/RobotDefs.h"
#include "Modules/Network/NetworkModule.h"

using namespace cv;

VisionModule::VisionModule(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    ballDetector = new BallDetector(spellBook);
    localizer = new Localizer(spellBook);
    capture = new CombinedCamera();
    top.Update(CAM_W, CAM_H);
    bottom.Update(CAM_W, CAM_H);
    message = new ImageMessage("Bottom", CAM_W, CAM_H, IMAGE_TYPE_BGR);
    frameWriter = NULL;
}

VisionModule::~VisionModule()
{
    delete frameWriter;
    delete ballDetector;
    delete localizer;
    delete message;
}

void VisionModule::Tick(float ellapsedTime)
{
    //top.ReadFromYUV422(capture->getFrameTop(), spellBook->perception.vision.BGR, spellBook->perception.vision.HSV, spellBook->perception.vision.GRAY, true);
    bottom.ReadFromYUV422(capture->getFrameBottom(), spellBook->perception.vision.BGR, spellBook->perception.vision.HSV, spellBook->perception.vision.GRAY, false);
    if(spellBook->perception.vision.BGR)
    {
        if(combinedImage.empty())
        {
            combinedImage.create(top.BGR.rows+bottom.BGR.rows+40, bottom.BGR.cols, CV_8UC3);
        }
        //top.BGR.copyTo(combinedImage(Rect(0, 0, top.BGR.cols, top.BGR.rows)));
        bottom.BGR.copyTo(combinedImage(Rect(0, bottom.BGR.rows+40, bottom.BGR.cols, bottom.BGR.rows)));

        if(spellBook->perception.vision.Record)
        {
            if(frameWriter == NULL)
            {
                SAY("Recording Video");
                frameWriter = new FrameWriter("video.avi", 15, combinedImage.size());
            }
            frameWriter->write(combinedImage);
        }

        //uint8_t *msgData = message->getData();
        //memcpy(msgData, bottom.BGR.data, message->getDataSize());
        //NetworkModule::SendMessage(message);
    }

    if(spellBook->perception.vision.ball.Enabled)
        ballDetector->Tick(ellapsedTime, top, bottom, combinedImage);
    if(spellBook->perception.vision.localization.Enabled)
        localizer->Tick(ellapsedTime, top, bottom, combinedImage);
}