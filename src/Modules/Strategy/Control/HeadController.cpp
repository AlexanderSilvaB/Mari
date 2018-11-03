#include "HeadController.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/RelativeCoord.h"

HeadController::HeadController(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    flip = false;
    closeInPitch = Deg2Rad(17);
    farPitch = Deg2Rad(0);

    ballCloseIn = true;
    ballCloseInDistance = 0.8f;

    numFramesTracked = 0;
    scanPitch = 0;
}

HeadController::~HeadController()
{
    
}

void HeadController::Tick(float ellapsedTime, const SensorValues &sensor)
{
    numFramesTracked += 1;

<<<<<<< HEAD
    //float CONSTANT_X = (float)CAM_BALL_W / H_DOF;
    //float xDiff = -(spellBook->perception.vision.ball.ImageX - (CAM_BALL_W / 2)) / CONSTANT_X;
    //spellBook->motion.HeadYaw = xDiff - sensor.joints.angles[Joints::HeadYaw];
    spellBook->motion.HeadYaw = Deg2Rad(0);

    //float CONSTANT_Y = (float)CAM_BALL_H / V_DOF;
    //float yDiff = (spellBook->perception.vision.ball.ImageY - (CAM_BALL_H / 2)) / CONSTANT_Y;
    //spellBook->motion.HeadPitch = yDiff - sensor.joints.angles[Joints::HeadPitch];
=======
    //float currHeadYaw = sensor.joints.angles[Joints::HeadYaw];
    //float currHeadPitch = sensor.joints.angles[Joints::HeadPitch];

    spellBook->motion.HeadYaw = spellBook->perception.vision.ball.HeadYaw;
    //spellBook->motion.HeadPitch = spellBook->perception.vision.ball.HeadPitch;
>>>>>>> master
    
    spellBook->motion.HeadPitch = Deg2Rad(20.0f);

    if(spellBook->perception.vision.ball.BallLostCount < 5)
    {
        if(spellBook->perception.vision.ball.BallDistance > 0.50f)
            spellBook->motion.HeadPitch = 0.0f;
        else
            spellBook->motion.HeadPitch = Deg2Rad(17.0f);

        float factor = abs(spellBook->motion.HeadYaw) / H_DOF;
        float speed = 0.25 * factor;  // 10.0 * factor * factor
        spellBook->motion.HeadSpeedYaw = speed;

        //factor = abs(spellBook->motion.HeadPitch) / V_DOF;
        //speed = 0.25 * factor;  // 10.0 * factor * factor
        spellBook->motion.HeadSpeedPitch = 0.2;

        spellBook->motion.HeadRelative = true;
    }
    else
    {
        //spellBook->motion.HeadYaw = calculateDesiredYaw(neckRelative);
        spellBook->motion.HeadPitch = scanPitch;
        spellBook->motion.HeadSpeedYaw = spellBook->strategy.HeadSearchSpeed;
        spellBook->motion.HeadSpeedPitch = spellBook->strategy.HeadSearchSpeed;
        spellBook->motion.HeadRelative = false;
        if(sensor.joints.angles[Joints::HeadYaw] < (spellBook->strategy.HeadYawRange - Deg2Rad(5.0f)) && !flip)
        {
            spellBook->motion.HeadYaw = spellBook->strategy.HeadYawRange;
            flip = false;
        }
        else if(sensor.joints.angles[Joints::HeadYaw] > (spellBook->strategy.HeadYawRange - Deg2Rad(5.0f)) && !flip)
        {
            flip = true;
        }
        else if(sensor.joints.angles[Joints::HeadYaw] > -(spellBook->strategy.HeadYawRange - Deg2Rad(5.0f)) && flip)
        {
            spellBook->motion.HeadYaw = -spellBook->strategy.HeadYawRange;
            flip = true;
        }
        else if(sensor.joints.angles[Joints::HeadYaw] < -(spellBook->strategy.HeadYawRange - Deg2Rad(5.0f)) && flip)
        {
            flip = false;
            scanPitch += Deg2Rad(20.0f);
            if(scanPitch > spellBook->strategy.HeadPitchRange)
                scanPitch = 0;
            spellBook->strategy.HeadScanCount++;
        }
    }
}

float HeadController::calculateDesiredPitch(CartesianCoord &neckRelative)
{
    float horizontalDistance = neckRelative.norm();

    if(ballCloseIn && horizontalDistance > 1.1 * ballCloseInDistance)
        ballCloseIn = false;

    if (!ballCloseIn && horizontalDistance < 0.9 * ballCloseInDistance)
        ballCloseIn = true;

    if(ballCloseIn)
        return closeInPitch;
    else
        return farPitch;
}

float HeadController::calculateDesiredYaw(CartesianCoord &neckRelative)
{
    return neckRelative.angle();
}
        