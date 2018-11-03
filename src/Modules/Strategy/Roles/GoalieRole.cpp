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
    spellBook->strategy.HeadYawRange = Deg2Rad(30.0f);
    spellBook->strategy.HeadPitchRange = Deg2Rad(20.0f);
    spellBook->strategy.HeadSearchSpeed = 0.05f;
    
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
            if(abs(spellBook->perception.vision.ball.HeadYaw) > Deg2Rad(5.0f))
            {
                spellBook->motion.Vy = 0.01f * SIG(spellBook->perception.vision.ball.HeadYaw);
                spellBook->motion.DefenderCentre = false;
            }
            else
            {
                spellBook->motion.Vy = 0.0f;
                if(spellBook->perception.vision.ball.BallDistance < 1.0f)
                {
                    spellBook->motion.DefenderCentre = true;
                }
                else
                {
                    spellBook->motion.DefenderCentre = false;
                }
            }
            cout << "Vy: " << spellBook->motion.Vy << endl;
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