#include "DefenderRole.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CartesianCoord.h"

DefenderRole::DefenderRole(SpellBook *spellBook) : InnerModule(spellBook)
{

    onBall = false;
    onPosition = false;
}
DefenderRole::~DefenderRole()
{
}
void DefenderRole::Tick(float ellapsedTime, const SensorValues &sensor)
{
    if ((spellBook->strategy.GameState == GC::READY || spellBook->strategy.GameState == GC::PLAYING) &&
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
    if (spellBook->strategy.GameState == GC::PLAYING)
    {
        spellBook->motion.KickRight = false;
        CartesianCoord coord;
        RelativeCoord rr;
        if (spellBook->perception.vision.ball.BallDetected)
        {

            rr.fromPixel(spellBook->perception.vision.ball.ImageX, spellBook->perception.vision.ball.ImageY, sensor.joints.angles[Joints::HeadYaw], -sensor.joints.angles[Joints::HeadPitch]);
            rr.toCartesian(coord, sensor.joints.angles[Joints::HeadYaw], sensor.joints.angles[Joints::HeadPitch]);
            cout << "X: " << coord.getX() << endl;
            cout << "Y: " << coord.getY() << endl;
            if (rr.getDistance() > 0.5f || !spellBook->perception.vision.ball.BallDetected)
            {
                spellBook->motion.Vx = (coord.getX() * 0.1);
                spellBook->motion.Vy = (coord.getY() * 0.1);
                if (abs(rr.getYaw()) > Deg2Rad(4))
                {
                    spellBook->motion.Vth = rr.getYaw() * -0.5f;
                }
                else
                {
                    spellBook->motion.Vth = 0;
                    spellBook->motion.Vx = (coord.getX() * 0.1);
                    spellBook->motion.Vy = 0;
                }
            }
            if (rr.getDistance() < 0.3f)
            {
                cout << "ENTROU NA MERDA DO IF E A DISTANCIA É: " << rr.getDistance();
                spellBook->motion.KickRight = true;
                spellBook->motion.Vth = 0;
                spellBook->motion.Vx = 0;
                spellBook->motion.Vy = 0;
            }
            cout << "NÃO ENTROU NA MERDA DO IF E A DISTANCIA É: " << rr.getDistance() << endl;
        }
        cout << "NÃO ENTROU NA MERDA DO IF E A DISTANCIA É: " << rr.getDistance() << endl;
    }
    /*else if (rr.getDistance() < 0.4f)
            {

                spellBook->strategy.WalkForward = false;
                onBall = true;
            }*/
    else
    {
        //spellBook->motion.KickLeft=false;
        //spellBook->motion.KickRight=false;
    }
}
