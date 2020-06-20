#include "Core/Utils/Math.h"
#include "BallDetector.h"
#include "Core/InitManager.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CombinedCamera.hpp"

#include "perception/vision/CameraToRR.hpp"
#include "perception/vision/WhichCamera.hpp"

#include <fstream>

#define R 2.0f
#define PI_2 1.5707963267f
#define TICK_TIME 30

using namespace cv;

BallDetector::BallDetector(SpellBook *spellBook)
    : InnerModule(spellBook)
{

    if (cascade.load("/home/nao/data/vision/cascade.xml"))
        cout << "Cascade file loaded" << endl;
    else
        cout << "Cascade file not found" << endl;

    targetYaw = 0;
    targetPitch = 0;

    method = CASCADE;

    colorsTxt = new int[256 * 256 * 256];
    for (int i = 0; i < 256 * 256 * 256; i++)
    {
        colorsTxt[i] = -1;
    }
    Load("/home/nao/data/vision/clustering.txt");
}

void BallDetector::Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage)
{
    spellBook->perception.vision.BGR = true;
    spellBook->perception.vision.HSV = true;
    spellBook->perception.vision.GRAY = true;

    Blackboard *blackboard = InitManager::GetBlackboard();
    SensorValues sensor = readFrom(motion, sensors);

    bool detected = false;
    switch (method)
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

    spellBook->perception.vision.ball.ImageX = ball.x;
    spellBook->perception.vision.ball.ImageY = ball.y;
    spellBook->perception.vision.ball.BallDetected = detected;
    if (detected)
    {
        spellBook->perception.vision.ball.BallLostCount = 0;
        spellBook->perception.vision.ball.TimeSinceBallSeen = 0;

        float currHeadYaw = sensor.joints.angles[Joints::HeadYaw];
        float currHeadPitch = sensor.joints.angles[Joints::HeadPitch];

        RelativeCoord rr;
        rr.fromPixel(ball.x, ball.y, currHeadYaw, currHeadPitch);
        spellBook->perception.vision.ball.BallYaw = rr.getYaw();
        spellBook->perception.vision.ball.BallDistance = rr.getDistance();
        spellBook->perception.vision.ball.BallPitch = rr.getPitch();

        float CONSTANT_X = (float)CAM_BALL_W / H_DOF;
        float xDiff = -(ball.x - (CAM_BALL_W / 2)) / CONSTANT_X;
        spellBook->perception.vision.ball.HeadYaw = xDiff - currHeadYaw;

        float CONSTANT_Y = (float)CAM_BALL_H / V_DOF;
        float yDiff = (ball.y - (CAM_BALL_H / 2)) / CONSTANT_Y;
        spellBook->perception.vision.ball.HeadPitch = yDiff - currHeadPitch;

        //cout << rr.getDistance() << "m, " << Rad2Deg(rr.getYaw()) << "º" << endl;
    }
    else
    {
        spellBook->perception.vision.ball.BallLostCount++;
        spellBook->perception.vision.ball.TimeSinceBallSeen += ellapsedTime;
    }
}

bool BallDetector::CascadeMethod(CameraFrame &top, CameraFrame &bottom)
{
    vector<Rect> balls;
    //cv::Mat hist;
    cv::Mat mask;
    cv::Mat rgb, hsv;
    rgb = bottom.BGR;
    Clustering(rgb);
    cvtColor(rgb, hsv, CV_BGR2HSV);
    //cv::equalizeHist(bottom.GRAY, hist);

    //Rinobot
    //inRange(bottom.HSV, Scalar(56, 102, 25), Scalar(116, 255, 255), mask);

    //cv::Mat frame;
    //frame = bottom.RGB;

    //LARC
    // #define GR_HUE_MIN  38
    // #define GR_HUE_MAX  75
    // #define GR_SAT_MIN  50
    // #define GR_SAT_MAX  255
    // #define GR_VAL_MIN  50
    // #define GR_VAL_MAX  255
    inRange(hsv, Scalar(38, 50, 50), Scalar(75, 255, 255), mask);

    cascade.detectMultiScale(bottom.GRAY, balls, 2, 1, 0, Size(16, 16));
    //cascade.detectMultiScale(hist, balls, 1.3, 5, 8, Size(16, 16));
    //cascade.detectMultiScale(gray, balls, 1.1, 5, 8, cv::Size(16, 16));
    if (balls.size() == 0)
        return false;

    // Gambiarra
    double raio = balls[0].width / 2.0;
    cv::Point pt(balls[0].x + raio, balls[0].y + raio);
    ball.radius = raio;
    ball.x = pt.x;
    ball.y = pt.y;

    return true;

    // //COMENTAR TUDO DAQUI PRA BAIXO CASO DÊ PROBLEMA!!!!!!!!!!!!!!!!!
    // // ctrl + kc - comenta
    // // ctrl + ku - descomenta
    // //

    //     double melhorRaio;
    //     double melhorConfidence = -1; // -1 ele continuar mostrando todos candidatos, =0
    //     cv::Point melhorPt;
    //     int step = 20;
    //     double x;
    //     double y;
    //     double raio2;
    //     for (int j = 0; j < balls.size(); j++)
    //     {

    //         double raio = balls[j].width / 2.0;
    //         cv::Point pt(balls[j].x + raio, balls[j].y + raio);
    //         raio2 = raio * 1.2;

    //         double confidence = 0;
    //         //quantos dos pontos analisados tem a cor verde (a cor branca da mask)
    //         int contverde = 0;

    //         //i++ e i+=10 tem diferenca, com ++ eu analizo 360 pontos e com 10, 10 ptos
    //         for (int i = 0; i < 360; i += step)
    //         {
    //             //conversao para de grau p rad 0.0174532 = pi/180
    //             x = raio2 * cos(i * 0.0174532) + pt.x;
    //             y = raio2 * sin(i * 0.0174532) + pt.y;

    //             if (mask.at<uchar>((int)y, (int)x) > 0)
    //             {
    //                 contverde++;
    //             }
    //         }
    //         cout << "Contverde: " << contverde << endl;
    //         //calculo de confianca no valor de acordo com o parametro (a quantidade de verde, só que é branco pela mask)
    //         if(contverde > 5)
    //         {
    //             confidence = contverde / (360.0 / step);
    //             if (confidence > melhorConfidence)
    //             {
    //                 melhorConfidence = confidence;
    //                 melhorRaio = raio;
    //                 melhorPt = pt;
    //             }
    //        }
    //     }
    //     if (melhorConfidence <= 0)
    //     {
    //         //cout << "seria uma bola?" << "\t" << melhorConfidence << endl;
    //         return false;
    //     }
    //     //cout << "Encontrado: " << melhorPt << " [ " << melhorRaio << " ] " << "\t" << (melhorConfidence * 100) << "%" << endl;

    //     ball.radius = melhorRaio;
    //     ball.x = melhorPt.x;
    //     ball.y = melhorPt.y;
    //     return true;
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

void BallDetector::Clustering(cv::Mat img)
{
    int red, green, blue;
    int col;

    for (int y = 0; y < img.rows; y++)
    {
        for (int x = 0; x < img.cols; x++)
        {
            cv::Vec3b color = img.at<cv::Vec3b>(y, x);
            blue = (int)color.val[0];
            green = (int)color.val[1];
            red = (int)color.val[2];
            col = (blue << 16) | (green << 8) | red;

            if (colorsTxt[col] != -1)
            {
                color.val[0] = 0;
                color.val[1] = 255;
                color.val[2] = 0;
            }
            img.at<cv::Vec3b>(y, x) = color;
        }
    }
}

void BallDetector::Load(std::string file)
{
    int color;
    ifstream inFile(file.c_str(), ios::in);
    if (inFile)
    {
        while (inFile >> color)
        {
            colorsTxt[color] = RED;
        }
        inFile.close();
    }
    else
        cout << "could not open file" << endl;
}