#include "RinoPlayer.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/CartesianCoord.h"

RinoPlayer::RinoPlayer(SpellBook *spellBook) : InnerModule(spellBook)
{
    onBall = false;
    onPosition = false;
    contPerdido = 0;
    scanPitch = 0;
    time = 0;
}
RinoPlayer::~RinoPlayer()
{
}
void RinoPlayer::Tick(float ellapsedTime, const SensorValues &sensor)
{
    CartesianCoord coord;
    RelativeCoord rr;
    if ((spellBook->strategy.GameState == GC::READY || spellBook->strategy.GameState == GC::PLAYING) &&
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
    if (spellBook->strategy.GameState == GC::PLAYING)
    {
        spellBook->motion.KickRight = false;
        spellBook->motion.Vth = 0;
        spellBook->motion.Vx = 0;
        spellBook->motion.Vy = 0;
        if (!spellBook->perception.vision.ball.BallDetected)
        {
            if(time <= 60 * ellapsedTime)
            {
                spellBook->motion.Vy = 0.05f;
                time+= ellapsedTime;
            }
            else if(time > 60 * ellapsedTime && time < 240 * ellapsedTime)
            {
                spellBook->motion.Vy = -0.05f;
                time+= ellapsedTime;

            }
            else
            {
                time = 0;                
            }   
        }
    }
}
