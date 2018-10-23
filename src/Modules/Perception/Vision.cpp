#include "Vision.h"

using namespace cv;

Vision::Vision(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    ballDetector = new BallDetector(spellBook);
    localizer = new Localizer(spellBook);

    img = Mat::zeros(240, 320, CV_8UC3);   
    capture = new CombinedCamera();

    frame.Update(320, 240);
}

Vision::~Vision()
{
    delete ballDetector;
    delete localizer;
}

void Vision::Tick(float ellapsedTime)
{
    const uint8_t *yuvData = capture->getFrameBottom();
    frame.ReadFromYUV422(yuvData);
    img.data = frame.GetDataBGR();
    //cv::Mat rawYuv(240, 320, CV_8UC2, yuvData);
    //img = imdecode(rawYuv, CV_LOAD_IMAGE_COLOR);
    //cv::cvtColor(rawYuv, img, CV_YUV2BGR_Y422);

    if(spellBook->perception.ball.Enabled)
        ballDetector->Tick(ellapsedTime, img);
    if(spellBook->perception.localization.Enabled)
        localizer->Tick(ellapsedTime);
}