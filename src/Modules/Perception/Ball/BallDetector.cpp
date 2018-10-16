#include "Core/Utils/Math.h"
#include "BallDetector.h"
#include "Core/InitManager.h"

#if USE_UNSW
#include "perception/vision/CameraToRR.hpp"
#endif

#define R 2.0f
#define PI_2 1.5707963267f
#define TICK_TIME 30

using namespace cv;

BallDetector::BallDetector(SpellBook *spellBook)
{
    this->spellBook = spellBook;


    w_ = -1;
    h_ = -1;

    angles[0] = angles[1] = 0.0f;

    if(cascade.load("/home/nao/data/vision/cascade.xml"))
        cout << "Cascade file loaded" << endl;
    else
        cout << "Cascade file not found" << endl;

    ballYaw = 0;
    ballPitch = 0;

    closeInPitch = Deg2Rad(17.0f);
    farPitch = Deg2Rad(0);

    ballCloseIn = true;
    ballCloseInDistance = 800.0f;

    targetYaw = 0;
    targetPitch = closeInPitch;
    ballLostCount = 0;

    method = CASCADE;
}

#ifdef USE_UNSW
float BallDetector::CalculateDesiredPitch(XYZ_Coord &neckRelativeTarget)
{
    float xSq = neckRelativeTarget.x * neckRelativeTarget.x;
    float ySq = neckRelativeTarget.y * neckRelativeTarget.y;
    float horizontalDistance = sqrtf(xSq + ySq);

    if (ballCloseIn && horizontalDistance > 1.1 * ballCloseInDistance)
        ballCloseIn = false;

    if (!ballCloseIn && horizontalDistance < 0.9 * ballCloseInDistance)
        ballCloseIn = true;

    if (ballCloseIn)
        return closeInPitch;
    return farPitch;
}

float BallDetector::CalculateDesiredYaw(XYZ_Coord &neckRelativeTarget)
{
    return atan2(neckRelativeTarget.y, neckRelativeTarget.x);
}
#endif

void BallDetector::Tick(float ellapsedTime, cv::Mat &img)
{
    img.copyTo(this->img);

    if(w_ < 0)
        w_ = 2.0f / img.cols;
    if(h_ < 0)
        h_ = 2.0f / img.rows;

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
        spellBook->perception.ball.HeadRelative = true;
        ballLostCount++;
        #ifdef USE_UNSW
        if(ballLostCount > 3)
        {
            targetPitch = CalculateDesiredPitch(neckRelative);
            ballYaw = CalculateDesiredYaw(neckRelative);
            targetYaw = ballYaw;
            float speed = 0.25f;

            spellBook->perception.ball.HeadRelative = false;
            spellBook->perception.ball.BallAzimuth = 0;
            spellBook->perception.ball.BallElevation = 0;
            spellBook->perception.ball.BallDistance = 0.0f;
            spellBook->perception.ball.HeadSpeed = speed;
        }
        #endif
    }
    else
    {
        cv::Point pt(ball.x, ball.y);
        cv::circle(frame, pt, ball.radius, cv::Scalar(0, 0, 255), CV_FILLED);
        #ifdef USE_UNSW
        Blackboard *blackboard = InitManager::GetBlackboard();
        conv_rr_.pose = readFrom(motion, pose);
        conv_rr_.updateAngles(readFrom(kinematics, sensorsLagged));
        RRCoord ballPosRR = conv_rr_.convertToRR(ball.x, ball.y + TOP_IMAGE_ROWS, true);
        targetYaw = ballPosRR.heading() - Deg2Rad(20);
        float factor = abs(targetYaw) / 1.06290551;
        speed = 0.75 * factor;
        ballLostCount = 0;
        neckRelative = conv_rr_.pose.robotRelativeToNeckCoord(ballPosRR, BALL_RADIUS);
        targetPitch = CalculateDesiredPitch(neckRelative);
        distance = ballPosRR.distance() / 1000.0f;
        #else
        #ifdef USE_QIBUILD    
        // Azimuth
        x_ = 1.0f - pt.x * w_;
        yc = sqrt(R*R - x_*x_);
        alpha = atan2(yc, x_);
        if(x_ == 0)
            targetYaw = 0;
        else if(x_ < 0)
            targetYaw = -(alpha - PI_2);
        else 
            targetYaw = -(-PI_2 + alpha);


        // Elevation
        y_ = pt.y * h_ - 1.0f;
        //targetPitch = y_*Deg2Rad(24.06f);       
        targetPitch = 0;
        distance = 0.2f;
        speed = 0.2f;
        #endif
        #endif

        cout << "Found: " << pt << " [ " << ball.radius << " ] | [" << Rad2Deg(targetYaw) << "º, " << Rad2Deg(targetPitch) << "º]" << endl;

        spellBook->perception.ball.BallDetected = true;
        spellBook->perception.ball.HeadRelative = true;
        spellBook->perception.ball.BallAzimuth = targetYaw;
        spellBook->perception.ball.BallElevation = targetPitch;
        spellBook->perception.ball.BallDistance = distance;
        spellBook->perception.ball.TimeSinceBallSeen = 0.0f;
        spellBook->perception.ball.HeadSpeed = speed;
    }

    #ifdef USE_V4L2
    #else
    #ifdef USE_QIBUILD    
    Capture::Show("frame", frame);
    Capture::Wait(1);
    #else
    cv::imshow("frame", frame);
    cv::waitKey(1);
    #endif
    #endif
}

bool BallDetector::CascadeMethod()
{
    vector<cv::Rect> balls;
    cv::Mat gray;
    cv::cvtColor(img, gray, CV_BGR2GRAY );
    cascade.detectMultiScale(gray, balls, 1.1, 5, 8, cv::Size(16, 16));
    if (balls.size() == 0)
        return false;

    ball.radius = balls[0].width / 2.0f;
    ball.x = balls[0].x + ball.radius;
    ball.y = balls[0].y + ball.radius;
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