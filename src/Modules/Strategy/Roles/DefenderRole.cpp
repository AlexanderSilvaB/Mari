#include "DefenderRole.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/CartesianCoord.h"

DefenderRole::DefenderRole(SpellBook *spellBook) : Role(spellBook)
{

    onBall = false;
    onPosition = false;
    contPerdido = 0;
    scanPitch = 0;
    conta = 0;
    conta2 = 0;
    conta3 = 0;
    Deg = Deg2Rad(6.05);
    Vel = 0.1;
    reset = false;
}
DefenderRole::~DefenderRole()
{
}
void DefenderRole::Tick(float ellapsedTime, const SensorValues &sensor)
{
    if(spellBook->strategy.TimeSincePenalized < 5.0f && !reset)
    {
        reset = true;
        conta = 0;
        conta2 = 0;
        conta3 = 0;
        Deg = Deg2Rad(6.05);
        Vel = 0.1;
    }
    else if(spellBook->strategy.TimeSincePenalized >= 5.0f)
    {
        reset = false;
    }

    spellBook->strategy.MoveHead = false;
    CartesianCoord coord;
    RelativeCoord rr;
    if ((spellBook->strategy.GameState == STATE_READY || spellBook->strategy.GameState == STATE_PLAYING) &&
        !onPosition)
    {

        if (spellBook->perception.vision.localization.Enabled)
        {
            contPerdido = 0;
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
        if(!Kicking())
        {
            if(spellBook->perception.vision.ball.BallLostCount < 8)
            {
                spellBook->motion.Vy = 0;
                if(abs(spellBook->perception.vision.ball.BallYaw) > Deg2Rad(10.0f))
                {
                    spellBook->motion.Vth = -spellBook->perception.vision.ball.BallYaw * 0.5f;
                    spellBook->motion.Vx = 0;
                    CancelKick();
                }
                else
                {
                    if(abs(spellBook->perception.vision.ball.BallYaw) > Deg2Rad(5.0f))
                        spellBook->motion.Vth = -spellBook->perception.vision.ball.BallYaw * 0.4f;
                    else
                        spellBook->motion.Vth = 0;

                    if(spellBook->perception.vision.ball.BallDistance < 0.45f)
                        spellBook->motion.HeadPitch = Deg2Rad(24.0f);
                    else if(spellBook->perception.vision.ball.BallDistance > 0.5f)
                        spellBook->motion.HeadPitch = Deg2Rad(0.0f);
                    if(spellBook->perception.vision.ball.BallDistance > 0.25f)
                    {
                        spellBook->motion.Vx = min(spellBook->perception.vision.ball.BallDistance * 0.25f, 0.25f);
                        CancelKick();
                    }
                    else
                    {
                        spellBook->motion.Vx = 0;
                        if(spellBook->motion.HeadPitch > 0)
                        {
                            PrepareKick(spellBook->perception.vision.ball.BallYaw);
                        }
                        else
                        {
                            CancelKick();
                        }
                    }
                }
            } else 
            {
                spellBook->motion.KickRight = false;
                spellBook->motion.Vth = 0;
                spellBook->motion.Vx = 0;
                if(conta<250){
                    spellBook->motion.Vy = Vel;
                    spellBook->motion.Vth = Deg;
                } else if(conta<350) {
                    spellBook->motion.Vy = 0;
                    spellBook->motion.Vth = 0;
                } else {
                    conta = 0; 
                    conta2++; 
                }
                conta++;

                if(conta2 == 4){
                    Vel = -0.1;
                    Deg = Deg2Rad(1.6);
                }
                if(conta2 == 9){
                    Vel = 0.1;
                    Deg = Deg2Rad(6.05);
                    conta2 = 0;
                }

                if(conta3 < 50){
                    spellBook->motion.HeadPitch = Deg2Rad(10);
                } else if (conta3 <100){
                    spellBook->motion.HeadPitch = Deg2Rad(20);
                } else if (conta3 <150){
                    spellBook->motion.HeadPitch = 0;
                } else {
                    conta3 = 0;
                }
                conta3++;
            }
        }
    }
}
