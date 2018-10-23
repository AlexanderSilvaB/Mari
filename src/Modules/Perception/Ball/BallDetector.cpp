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

void BallDetector::Tick(float ellapsedTime, cv::Mat &img)
{
    img.copyTo(this->img);
    cv::Mat frame = img.clone();

    bool detected = false;
    switch(method)
    {
        case CASCADE:
            detected = CascadeMethod();
            break;
        case GEOMETRIC:
            detected = GeometricMethod();
            break;
        case NEURAL:
            detected = NeuralMethod();
            break;
        default:
            break;
    }
    if(!detected)
    {
        cout << "Not found" << endl;
        spellBook->perception.ball.BallDetected = false;
        spellBook->perception.ball.TimeSinceBallSeen += ellapsedTime;
        spellBook->perception.ball.HeadRelative = false;
        if(spellBook->perception.ball.TimeSinceBallSeen > 1.0f)
        {
            targetPitch = 0.0f;
            targetYaw = 0.0f;
            speed = 0.25f;

            spellBook->perception.ball.HeadRelative = false;
            spellBook->perception.ball.BallAzimuth = 0;
            spellBook->perception.ball.BallElevation = 0;
            spellBook->perception.ball.BallDistance = 0.0f;
            spellBook->perception.ball.HeadSpeed = speed;
        }
    }
    else
    {
        cv::Point pt(ball.x, ball.y);
        //cv::circle(frame, pt, ball.radius, cv::Scalar(0, 0, 255), CV_FILLED);
        Blackboard *blackboard = InitManager::GetBlackboard();
        SensorValues sensor = readFrom(kinematics, sensorsLagged);
        RelativeCoords ballPosRR;
        float currHeadYaw = sensor.joints.angles[Joints::HeadYaw];
        float currHeadPitch = sensor.joints.angles[Joints::HeadPitch];
        ballPosRR.fromPixel(ball.x, ball.y, currHeadYaw, currHeadPitch);
        targetYaw = ballPosRR.getYaw();
        targetPitch = ballPosRR.getPitch();
        distance = ballPosRR.getDistance();
        float factor = abs(targetYaw) / H_FOV;
        speed = 0.75 * factor;

        cout << "Found: " << pt << " [ " << ball.radius << " ] | [" << Rad2Deg(targetYaw) << "º, " << Rad2Deg(targetPitch) << "º] " << distance << "m" << endl;

        spellBook->perception.ball.BallDetected = true;
        spellBook->perception.ball.HeadRelative = false;
        spellBook->perception.ball.BallAzimuth = targetYaw;
        spellBook->perception.ball.BallElevation = targetPitch;
        spellBook->perception.ball.BallDistance = distance;
        spellBook->perception.ball.TimeSinceBallSeen = 0.0f;
        spellBook->perception.ball.HeadSpeed = speed;
    }
}

bool BallDetector::CascadeMethod()
{
    vector<Rect> balls;
    cv::Mat gray;
    cv::Mat hist;
    cv::Mat hsv;
    cv::Mat mask;
    cv::cvtColor(img, gray, CV_BGR2GRAY);
    cv::equalizeHist(gray,hist);
    cv::cvtColor(img, hsv, CV_BGR2HSV);
    inRange(hsv, Scalar(56, 102, 25), Scalar(116, 255, 255), mask);
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
bool BallDetector::GeometricMethod()
{
    cv::Mat hsv;
    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);
    cv::imshow("hsv", hsv);

    cv::Scalar low(0, 0, 0);
    cv::Scalar high(180, 255, 140);

    cv::Mat black;
    cv::inRange(hsv, low, high, black);
    cv::imshow("black", black);

    return false;
}

bool BallDetector::NeuralMethod()
{
    return false;
}