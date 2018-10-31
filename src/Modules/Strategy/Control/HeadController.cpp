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

    float CONSTANT_X = (float)CAM_W / H_DOF;
    float xDiff = -(spellBook->perception.vision.ball.ImageX - (CAM_W / 2)) / CONSTANT_X;
    spellBook->motion.HeadYaw = xDiff - sensor.joints.angles[Joints::HeadYaw];

    float CONSTANT_Y = (float)CAM_H / V_DOF;
    float yDiff = (spellBook->perception.vision.ball.ImageY - (CAM_H / 2)) / CONSTANT_Y;
    spellBook->motion.HeadPitch = yDiff - sensor.joints.angles[Joints::HeadPitch];
    
    if(spellBook->perception.vision.ball.BallLostCount < 5)
    {
        float factor = abs(spellBook->motion.HeadYaw) / H_DOF;
        float speed = 0.25 * factor;  // 10.0 * factor * factor
        spellBook->motion.HeadSpeedYaw = speed;

        factor = abs(spellBook->motion.HeadPitch) / V_DOF;
        speed = 0.25 * factor;  // 10.0 * factor * factor
        spellBook->motion.HeadSpeedPitch = speed;

        spellBook->motion.HeadRelative = true;
    }
    else
    {
        //spellBook->motion.HeadYaw = calculateDesiredYaw(neckRelative);
        spellBook->motion.HeadPitch = Deg2Rad(scanPitch);
        spellBook->motion.HeadSpeedYaw = 0.1f;
        spellBook->motion.HeadSpeedPitch = 0.1f;
        spellBook->motion.HeadRelative = false;
        if(sensor.joints.angles[Joints::HeadYaw] < Deg2Rad(45.0f) && !flip)
        {
            spellBook->motion.HeadYaw = Deg2Rad(50.0f);
            flip = false;
        }
        else if(sensor.joints.angles[Joints::HeadYaw] > Deg2Rad(45.0f) && !flip)
        {
            flip = true;
        }
        else if(sensor.joints.angles[Joints::HeadYaw] > Deg2Rad(-45.0f) && flip)
        {
            spellBook->motion.HeadYaw = Deg2Rad(-50.0f);
            flip = true;
        }
        else if(sensor.joints.angles[Joints::HeadYaw] < Deg2Rad(-45.0f) && flip)
        {
            flip = false;
            scanPitch += 20;
            if(scanPitch > 20)
                scanPitch = 0;
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
        