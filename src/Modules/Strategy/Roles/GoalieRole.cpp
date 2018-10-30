#include "GoalieRole.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CartesianCoord.h"

GoalieRole::GoalieRole(SpellBook *spellBook)
    : InnerModule(spellBook)
{
    onGoal = false;
}

GoalieRole::~GoalieRole()
{

}

void GoalieRole::Tick(float ellapsedTime, const SensorValues &sensor)
{
    if((spellBook->strategy.GameState == GC::READY || spellBook->strategy.GameState == GC::PLAYING) &&
        !onGoal)
    {
        if(spellBook->perception.vision.localization.Enabled)
        {
            spellBook->strategy.WalkForward = true;
            spellBook->strategy.TargetX = 0.0f;
            spellBook->strategy.TargetY = 0.0f;
            spellBook->strategy.TargetTheta = 0;

            CartesianCoord current(spellBook->perception.vision.localization.X, spellBook->perception.vision.localization.Y); 
            CartesianCoord desired(spellBook->strategy.TargetX, spellBook->strategy.TargetY); 
            if(current.distance(desired)  < 0.2f)
            {
                spellBook->strategy.WalkForward = false;
                onGoal = true;
            }
        }
        else
        {
            onGoal = true;
        }
    }
    if(spellBook->strategy.GameState == GC::PLAYING && onGoal)
    {
        if(spellBook->perception.vision.ball.BallDetected)
        {
            RelativeCoord rr;
            rr.fromPixel(spellBook->perception.vision.ball.ImageX, spellBook->perception.vision.ball.ImageY, sensor.joints.angles[Joints::HeadYaw], -sensor.joints.angles[Joints::HeadPitch]);
            CartesianCoord coord;
            rr.toCartesian(coord, sensor.joints.angles[Joints::HeadYaw], sensor.joints.angles[Joints::HeadPitch]);
            cout << "X: " << coord.getX() << endl;
            cout << "Y: " << coord.getY() << endl;

            if(spellBook->perception.vision.localization.Enabled)
            {
                spellBook->strategy.WalkAside = true;
                spellBook->strategy.TargetX = spellBook->perception.vision.localization.X;
                spellBook->strategy.TargetY = spellBook->perception.vision.localization.Y + coord.getY();
                spellBook->strategy.TargetTheta = spellBook->perception.vision.localization.Theta;
            }
            else if(abs(coord.getY()) > 0.1f)
            {
                if(coord.getY() > 0)
                    spellBook->motion.Vy = min(coord.getY()*0.5f, 0.5f);
                else
                    spellBook->motion.Vy = max(coord.getY()*0.5f, -0.5f);
                //spellBook->motion.Vy = 0;
            }
            else
            {
                spellBook->motion.Vy = 0.0f;
            }
            cout << "D: " << rr.getDistance() << endl;
            if(abs(rr.getYaw()) < Deg2Rad(5.0f) && rr.getDistance() < 0.7f)
            {
                spellBook->motion.DefenderCentre = true;
            }
            else
            {
                spellBook->motion.DefenderCentre = false;
            }
        }
        else
        {
            if(spellBook->perception.vision.localization.Enabled)
                spellBook->strategy.WalkAside = false;
            else
                spellBook->motion.Vy = 0.0f;
            spellBook->motion.DefenderCentre = false;
        }
    }
}