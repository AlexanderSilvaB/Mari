#include "Core/Utils/Math.h"
#include "BallDetector.h"
#include "Core/InitManager.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/RelativeCoords.h"

#define R 2.0f
#define PI_2 1.5707963267f
#define TICK_TIME 30

using namespace cv;

BallDetector::BallDetector(SpellBook *spellBook)
    :   InnerModule(spellBook)
{

    if(cascade.load("/home/nao/data/vision/cascade.xml"))
        cout << "Cascade file loaded" << endl;
    else
        cout << "Cascade file not found" << endl;

    targetYaw = 0;
    targetPitch = 0;

    method = CASCADE;
}

void BallDetector::Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom)
{
    spellBook->perception.vision.BGR = true;
    spellBook->perception.vision.HSV = true;
    spellBook->perception.vision.GRAY = true;
    
    bool detected = false;
    switch(method)
    {
        case CASCADE:
            detected = CascadeMethod(top, bottom);
            break;
        case GEOMETRIC:
            detected = GeometricMethod(top, bottom);
            break;
        case NEURAL:
            detected = NeuralMethod(top, bottom);
            break;
        default:
            break;
    }
    if(!detected)
    {
        //cout << "Not found" << endl;
        spellBook->perception.vision.ball.BallDetected = false;
        spellBook->perception.vision.ball.TimeSinceBallSeen += ellapsedTime;
        spellBook->perception.vision.ball.HeadRelative = true;
        spellBook->perception.vision.ball.BallAzimuth = 0;
        spellBook->perception.vision.ball.BallElevation = 0;
        if(spellBook->perception.vision.ball.TimeSinceBallSeen > 1.0f)
        {
            targetPitch = 0.0f;
            targetYaw = 0.0f;
            speed = 0.25f;

            spellBook->perception.vision.ball.HeadRelative = false;
            spellBook->perception.vision.ball.BallAzimuth = 0;
            spellBook->perception.vision.ball.BallElevation = 0;
            spellBook->perception.vision.ball.BallDistance = 0.0f;
            spellBook->perception.vision.ball.HeadSpeed = speed;
        }
    }
    else
    {
        Blackboard *blackboard = InitManager::GetBlackboard();
        SensorValues sensor = readFrom(kinematics, sensorsLagged);
        float currHeadYaw = sensor.joints.angles[Joints::HeadYaw];
        float currHeadPitch = sensor.joints.angles[Joints::HeadPitch];

        RelativeCoords ballPosRR;
        //ballPosRR.fromPixel(ball.x, ball.y, currHeadYaw, currHeadPitch);
        ballPosRR.fromPixel(ball.x, ball.y);
        targetYaw = ballPosRR.getYaw();
        //targetPitch = ballPosRR.getPitch();
        distance = ballPosRR.getDistance();
        if(distance > 1.0f)
            targetPitch = 0.0f;
        else
            targetPitch = Deg2Rad(17.0f);
        float factor = abs(targetYaw) / (float)H_FOV;
        speed = 0.25f * factor;
        
        cout << "Found: " << ball.x << ", " << ball.y << " [ " << ball.radius << " ] | [" << Rad2Deg(targetYaw) << "º, " << Rad2Deg(ballPosRR.getPitch()) << "º] " << distance << "m | " << speed << endl;

        spellBook->perception.vision.ball.BallDetected = true;
        spellBook->perception.vision.ball.HeadRelative = true;
        spellBook->perception.vision.ball.BallAzimuth = -targetYaw;
        spellBook->perception.vision.ball.BallElevation = targetPitch;
        spellBook->perception.vision.ball.BallDistance = distance;
        spellBook->perception.vision.ball.TimeSinceBallSeen = 0.0f;
        spellBook->perception.vision.ball.HeadSpeed = speed;
    }
}

bool BallDetector::CascadeMethod(CameraFrame &top, CameraFrame &bottom)
{
    vector<Rect> balls;
    cv::Mat hist;
    cv::Mat mask;
    cv::equalizeHist(bottom.GRAY, hist);
    inRange(bottom.HSV, Scalar(56, 102, 25), Scalar(116, 255, 255), mask);
    cascade.detectMultiScale(hist, balls, 1.3, 5, 8, Size(16, 16));
    //cascade.detectMultiScale(gray, balls, 1.1, 5, 8, cv::Size(16, 16));
    if (balls.size() == 0)
        return false;

    double melhorRaio;
    double melhorConfidence = -1; // -1 ele continuar mostrando todos candidatos, =0
    cv::Point melhorPt;
    int step = 10;
    double x;
    double y;
    double raio2;
    for (int j = 0; j < balls.size(); j++)
    {

        double raio = balls[j].width / 2.0;
        cv::Point pt(balls[j].x + raio, balls[j].y + raio);
        raio2 = raio * 1.2;

        double confidence = 0;
        // quantos dos pontos analisados tem a cor verde (a cor branca da mask)
        int contverde = 0;

        //i++ e i+=10 tem diferenca, com ++ eu analizo 360 pontos e com 10, 10 ptos
        for (int i = 0; i < 360; i += step)
        {
            //conversao para de grau p rad 0.0174532 = pi/180
            x = raio2 * cos(i * 0.0174532) + pt.x;
            y = raio2 * sin(i * 0.0174532) + pt.y;

            if (mask.at<uchar>((int)y, (int)x) > 0)
            {
                contverde++;
            }
        }
        //calculo de confianca no valor de acordo com o parametro (a quantidade de verde, só que é branco pela mask)
        confidence = contverde / (360.0 / step);
        if (confidence > melhorConfidence)
        {
            melhorConfidence = confidence;
            melhorRaio = raio;
            melhorPt = pt;
        }
    }
    if (melhorConfidence <= 0)
    {
        cout << "seria uma bola?" << "\t" << melhorConfidence << endl;
        return false;
    }
    cout << "Encontrado: " << melhorPt << " [ " << melhorRaio << " ] " << "\t" << (melhorConfidence * 100) << "%" << endl;


    ball.radius = melhorRaio;
    ball.x = melhorPt.x;
    ball.y = melhorPt.y;
    return true;
}

cv::RNG rng(12345);
bool BallDetector::GeometricMethod(CameraFrame &top, CameraFrame &bottom)
{
    cv::Scalar low(0, 0, 0);
    cv::Scalar high(180, 255, 140);

    cv::Mat black;
    cv::inRange(bottom.HSV, low, high, black);
    cv::imshow("black", black);

    return false;
}

bool BallDetector::NeuralMethod(CameraFrame &top, CameraFrame &bottom)
{
    return false;
}