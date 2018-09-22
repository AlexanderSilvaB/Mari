#include "Vision.h"

using namespace cv;

Vision::Vision(SpellBook *spellBook)
{
    this->spellbook = spellbook;
    if(spellBook->perceptionSpell.EnableBallDetector)
        ballDetector = new BallDetector(spellBook);
    else
        ballDetector = NULL;

    #ifdef USE_V4L2
    img = Mat::zeros(240, 320, CV_8UC3);   
    capture = new CombinedCamera();
    #else
    #ifdef USE_QIBUILD
    capture = Robot::CreateModule<Capture>();
    #else
    capture.open(0);
    #endif
    #endif

    frame.Update(320, 240);
}

Vision::~Vision()
{
    delete ballDetector;
}

void Vision::Tick(float ellapsedTime)
{
    #ifdef USE_V4L2
    cout << "USE_V4L2" << endl;
    const uint8_t *yuvData = capture->getFrameBottom();
    frame.ReadFromYUV422(yuvData);
    memcpy(img.data, frame.GetDataBGR(), 320*240*3);
    //cv::Mat rawYuv(240, 320, CV_8UC2, yuvData);
    //img = imdecode(rawYuv, CV_LOAD_IMAGE_COLOR);
    //cv::cvtColor(rawYuv, img, CV_YUV2BGR_YVYU);
    imwrite("img.jpg", img);
    #else
    #ifdef USE_QIBUILD
    img = capture->Get();
    #else
    capture >> img;
    cv::flip(img, img, 1);
    cv::resize(img, img, cv::Size(320, 240));
    #endif
    #endif

    if(ballDetector != NULL)
        ballDetector->Tick(ellapsedTime, img);
}