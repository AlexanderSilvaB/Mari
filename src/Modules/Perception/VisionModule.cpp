#include "VisionModule.h"
#include "Core/Utils/RobotDefs.h"
#include "Modules/Network/NetworkModule.h"

using namespace cv;

VisionModule::VisionModule(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    ballDetector = new BallDetector(spellBook);
    localizer = new Localizer(spellBook);
    featureExtractor = new FeatureExtractor(spellBook);
    robotDetector = new RobotDetector(spellBook);

    capture = new CombinedCamera();
    top.Update(CAM_W, CAM_H);
    bottom.Update(CAM_W, CAM_H);
    frameWriter = NULL;
}

VisionModule::~VisionModule()
{
    delete frameWriter;
    delete ballDetector;
    delete localizer;
    delete featureExtractor;
    delete robotDetector;
}

void VisionModule::setControl(Camera *camera, const uint32_t controlId, const int32_t controlValue)
{
    camera->setControl(V4L2_CID_AUTO_WHITE_BALANCE, 1);
    camera->setControl(V4L2_CID_EXPOSURE_AUTO, 1);
    camera->setControl(controlId, controlValue);
    camera->setControl(V4L2_CID_AUTO_WHITE_BALANCE, 0);
    camera->setControl(V4L2_CID_EXPOSURE_AUTO, 0);
}

void VisionModule::Tick(float ellapsedTime)
{
    /*
    if(spellBook->network.SelectedCamera != 1)
    {
        //cout << "VisionModule" << endl;
        if(spellBook->network.botSettings.brightnessChanged)
        {
            setControl(capture->getCameraBot(), V4L2_CID_BRIGHTNESS, spellBook->network.botSettings.brightness);
            spellBook->network.botSettings.brightnessChanged = false;
        }
        if(spellBook->network.botSettings.saturationChanged)
        {
            setControl(capture->getCameraBot(), V4L2_CID_SATURATION, spellBook->network.botSettings.saturation);
            spellBook->network.botSettings.saturationChanged = false;
        }
        if(spellBook->network.botSettings.contrastChanged)
        {
            setControl(capture->getCameraBot(), V4L2_CID_CONTRAST, spellBook->network.botSettings.contrast);
            spellBook->network.botSettings.contrastChanged = false;
        }
        if(spellBook->network.botSettings.sharpnessChanged)
        {
            setControl(capture->getCameraBot(), V4L2_CID_SHARPNESS, spellBook->network.botSettings.sharpness);
            spellBook->network.botSettings.sharpnessChanged = false;
        }
    }
    if(spellBook->network.SelectedCamera != 0)
    {
        if(spellBook->network.topSettings.brightnessChanged)
        {
            setControl(capture->getCameraTop(), V4L2_CID_BRIGHTNESS, spellBook->network.topSettings.brightness);
            spellBook->network.topSettings.brightnessChanged = false;
        }
        if(spellBook->network.topSettings.saturationChanged)
        {
            setControl(capture->getCameraTop(), V4L2_CID_SATURATION, spellBook->network.topSettings.saturation);
            spellBook->network.topSettings.saturationChanged = false;
        }
        if(spellBook->network.topSettings.contrastChanged)
        {
            setControl(capture->getCameraTop(), V4L2_CID_CONTRAST, spellBook->network.topSettings.contrast);
            spellBook->network.topSettings.contrastChanged = false;
        }
        if(spellBook->network.topSettings.sharpnessChanged)
        {
            setControl(capture->getCameraTop(), V4L2_CID_SHARPNESS, spellBook->network.topSettings.sharpness);
            spellBook->network.topSettings.sharpnessChanged = false;
        }
    }
    */

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

        if(spellBook->network.TCPConnected)
        {
            if(spellBook->network.SelectedCamera == 0)
            {
                cvtColor(bottom.BGR, messageImage, COLOR_BGR2RGB);
                message.fromCV("Bottom", messageImage);
            }
            else if(spellBook->network.SelectedCamera == 1)
            {
                cvtColor(top.BGR, messageImage, COLOR_BGR2RGB);
                message.fromCV("Top", messageImage);
            }
            else
            {
                cvtColor(combinedImage, messageImage, COLOR_BGR2RGB);
                message.fromCV("Combined", combinedImage);
            }
            NetworkModule::SendMessage(&message);
        }
    }

    if(spellBook->perception.vision.ball.Enabled)
        ballDetector->Tick(ellapsedTime, top, bottom, combinedImage);
    if(spellBook->perception.vision.localization.Enabled)
        localizer->Tick(ellapsedTime, top, bottom, combinedImage);
    if(spellBook->perception.vision.feature.Enabled)
        featureExtractor->Tick(ellapsedTime, top, bottom, combinedImage);
    if(spellBook->perception.vision.robotDetector.Enabled)
        robotDetector->Tick(ellapsedTime, top, bottom, combinedImage);
}