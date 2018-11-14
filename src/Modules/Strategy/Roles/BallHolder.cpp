#include "BallHolder.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/CartesianCoord.h"

BallHolder::BallHolder(SpellBook *spellBook) : Role(spellBook)
{

    onBall = false;
    onPosition = false;
}
BallHolder::~BallHolder()
{
}
void BallHolder::Tick(float ellapsedTime, const SensorValues &sensor)
{
    CartesianCoord coord;
    RelativeCoord rr;
    spellBook->strategy.MoveHead = false;
    if ((spellBook->strategy.GameState == STATE_READY || spellBook->strategy.GameState == STATE_PLAYING) &&
        !onPosition)
    {

        if (spellBook->perception.vision.localization.Enabled)
        {
            spellBook->strategy.WalkForward = true;
            spellBook->strategy.TargetX = 0.0f;
            spellBook->strategy.TargetY = 0.0f;
            spellBook->strategy.TargetTheta = 0;

            CartesianCoord current(spellBook->perception.vision.localization.X, spellBook->perception.vision.localization.Y);
            CartesianCoord desired(spellBook->strategy.TargetX, spellBook->strategy.TargetY);
            if (current.distance(desired) < 0.2f)
            {
                spellBook->strategy.WalkForward = false;
                onPosition = true;
            }
        }
        else
        {
            onPosition = true;
        }
    }
    if (spellBook->strategy.GameState == STATE_PLAYING)
    {
        spellBook->motion.KickRight = false;
        spellBook->motion.KickLeft = false;
        spellBook->motion.Vth = 0;
        spellBook->motion.Vx = 0;
        spellBook->motion.Vy = 0;

        if (spellBook->perception.vision.ball.BallLostCount < 5)
        {
            cout << "ballLost" << endl;
            if(spellBook->perception.vision.ball.BallYaw > Deg2Rad(10))
            {
                spellBook->motion.Vth = - spellBook->perception.vision.ball.BallYaw * 0.5;

            }
            else if(spellBook->perception.vision.ball.BallYaw > Deg2Rad(5))
            {
                spellBook->motion.Vth = - spellBook->perception.vision.ball.BallYaw * 0.25;
            }
            else
            {
                spellBook->motion.Vth = 0;
                spellBook->motion.Vx = 0.04; //0.02
            }
            if (spellBook->perception.vision.ball.BallDistance > 0.5f)
            {
                cout << "maior que meio" << endl;
                spellBook->motion.Vth = -(spellBook->perception.vision.ball.BallYaw)*0.25;
                spellBook->motion.Vx = (spellBook->perception.vision.ball.BallDistance) * 0.2f;
                spellBook->motion.Vy = 0;
                spellBook->motion.HeadPitch = Deg2Rad(15.0f);
                spellBook->motion.HeadYaw = Deg2Rad(0);
            }
            else if (spellBook->perception.vision.ball.BallDistance < 0.3f)
            {
                cout << "menor que meio" << endl;
                spellBook->motion.HeadPitch = Deg2Rad(25.0f);
                spellBook->motion.HeadYaw = Deg2Rad(0);
                spellBook->motion.Vx = (spellBook->perception.vision.ball.BallDistance) * 0.5f +0.01;//0.01
                spellBook->motion.Vy = 0;
                spellBook->motion.Vth = -(spellBook->perception.vision.ball.BallYaw);
            }
        }
        else 
        {
            if (spellBook->perception.vision.ball.BallLostCount < 30)
            {
                cout<<"distancia"<< spellBook->perception.vision.ball.BallDistance<< endl;
                spellBook->motion.Vth = -(spellBook->perception.vision.ball.BallYaw) * 0.5f;
                spellBook->motion.Vy = 0;
                spellBook->motion.HeadPitch = Deg2Rad(15.0f);
                spellBook->motion.HeadYaw = Deg2Rad(0);
                if (spellBook->perception.vision.ball.BallDistance < 0.25)
                {
                    spellBook->motion.Vx = -(spellBook->perception.vision.ball.BallDistance) * 0.25f;
                }
                else
                {
                    spellBook->motion.Vx = (spellBook->perception.vision.ball.BallDistance) * 0.25f;
                }
            }
            else
            {
                cout << "to perdido" << endl;
                spellBook->motion.Vth = 0.02;
                spellBook->motion.Vx = 0.03;
                spellBook->motion.Vy = 0;
                spellBook->motion.HeadPitch = Deg2Rad(2.0);
                spellBook->motion.HeadYaw = 0.0;
                spellBook->motion.HeadSpeedYaw = 0.2f;
                spellBook->motion.HeadSpeedPitch = 0.2f;
            }
        }
    }
}