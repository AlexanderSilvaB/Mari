#include "HeadController.h"
#include "Core/Utils/Math.h"

HeadController::HeadController(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    closeInPitch = Deg2Rad(17);
    farPitch = Deg2Rad(0);

    ballCloseIn = true;
    ballCloseInDistance = 800.0f;

    numFramesTracked = 0;
}

HeadController::~HeadController()
{
    
}

void HeadController::Tick(float ellapsedTime)
{
    Blackboard *blackboard = InitManager::GetBlackboard();
    AbsCoord robotPos = readFrom(localisation, robotPos);
    AbsCoord ballPos = readFrom(localisation, ballPos);
    RRCoord ballPosRR = readFrom(localisation, ballPosRR);
    AbsCoord ballPosRRC = readFrom(localisation, ballPosRRC);
    uint32_t ballLostCount = readFrom(localisation, ballLostCount);
    XYZ_Coord neckRelative = readFrom(localisation, ballNeckRelative);
    std::vector<BallInfo> balls = readFrom(vision, balls);

    numFramesTracked += 1;
    bool lostBall = (ballLostCount > 60);
    spellBook->motion.HeadPitch = calculateDesiredPitch(neckRelative);

    if (balls.size() > 0)
    {
        BallInfo ball = balls[0];
        Point imgCoords = ball.imageCoords;

        float CONSTANT_X = 0;
        float xDiff = 0;
        if(ball.topCamera)
        {
            CONSTANT_X = 1280.0 / 1.06290551;
            xDiff = -(imgCoords[0] - 640) / CONSTANT_X;
        }
        else
        {
            CONSTANT_X = 640.0 / 1.06290551;
            xDiff = -(imgCoords[0] - 640 / 2) / CONSTANT_X;
        }
        spellBook->motion.HeadYaw = xDiff;

        if(ballLostCount < 3)
        {
            float factor = abs(spellBook->motion.HeadYaw) / 1.06290551;
            float speed = 0.75 * factor;  // 10.0 * factor * factor
            spellBook->motion.HeadSpeed = speed;
            spellBook->motion.HeadRelative = true;
        }
        else
        {
            spellBook->motion.HeadYaw = calculateDesiredYaw(neckRelative);
            spellBook->motion.HeadSpeed = 0.25f;
            spellBook->motion.HeadRelative = false;
        }
    }
}

float HeadController::calculateDesiredPitch(XYZ_Coord neckRelative)
{
    float xSq = neckRelative.x * neckRelative.x; 
    float ySq = neckRelative.y * neckRelative.y;
    float horizontalDistance = sqrt(xSq + ySq);

    if(ballCloseIn && horizontalDistance > 1.1 * ballCloseInDistance)
        ballCloseIn = false;

    if (!ballCloseIn && horizontalDistance < 0.9 * ballCloseInDistance)
        ballCloseIn = true;

    if(ballCloseIn)
        return closeInPitch;
    else
        return farPitch;
}

float HeadController::calculateDesiredYaw(XYZ_Coord neckRelative)
{
    return atan2(neckRelative.y, neckRelative.x);
}
        