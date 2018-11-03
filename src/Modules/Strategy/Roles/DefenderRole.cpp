#include "DefenderRole.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/CartesianCoord.h"

DefenderRole::DefenderRole(SpellBook *spellBook) : InnerModule(spellBook)
{

    onBall = false;
    onPosition = false;
    contPerdido = 0;
    scanPitch = 0;
}
DefenderRole::~DefenderRole()
{
}
void DefenderRole::Tick(float ellapsedTime, const SensorValues &sensor)
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
        if (spellBook->perception.vision.ball.BallDetected)
        {
            cout << "Valor atual do GetX: " << coord.getX() << endl;
            rr.fromPixel(spellBook->perception.vision.ball.ImageX, spellBook->perception.vision.ball.ImageY, sensor.joints.angles[Joints::HeadYaw], -sensor.joints.angles[Joints::HeadPitch]);
            rr.toCartesian(coord, sensor.joints.angles[Joints::HeadYaw], sensor.joints.angles[Joints::HeadPitch]);
            cout << "X: " << coord.getX() << endl;
            cout << "Y: " << coord.getY() << endl;
            cout << "tentativa numero: " << spellBook->perception.vision.ball.BallLostCount << endl;

            if (rr.getDistance() > 0.8f)
            {
                if (rr.getYaw() > Deg2Rad(5) || rr.getYaw() < Deg2Rad(-5))
                {
                    spellBook->motion.Vth = 0; //min(rr.getYaw() * rr.getDistance(), Deg2Rad(0.3f));
                    spellBook->motion.Vx = abs(min(coord.getX(), 0.1f));
                    spellBook->motion.Vy = min(coord.getY(), 0.05f) * SIG(rr.getYaw());
                }
                else
                {
                    spellBook->motion.Vth = 0;
                    spellBook->motion.Vx = (coord.getX() * 0.1);
                    spellBook->motion.Vy = 0;
                }
            }
            else if (rr.getDistance() > 0.5f && rr.getDistance() < 0.8f)
            {
                cout << "0.5<dist<0.8" << endl;
                if (rr.getYaw() > Deg2Rad(8) || rr.getYaw() < Deg2Rad(-8))
                {
                    spellBook->motion.Vth = 0; //min(rr.getYaw() * rr.getDistance(), Deg2Rad(0.1f));
                    spellBook->motion.Vx = abs(min(coord.getX(), 0.15f));
                    spellBook->motion.Vy = (coord.getY() * 0.05f) * SIG(rr.getYaw());
                }
                else
                {
                    cout << "dist<0.5" << endl;
                    spellBook->motion.Vth = 0;
                    spellBook->motion.Vx = abs((coord.getX() * 0.03));
                    spellBook->motion.Vy = 0;
                }
            }
            else
            {
                if (rr.getYaw() > Deg2Rad(7))
                {
                    spellBook->motion.Vth = rr.getYaw() * 0.02;
                    spellBook->motion.Vy = 0;
                    spellBook->motion.Vx = 0;
                }
                else
                {
                    spellBook->motion.Vx = coord.getX() * 0.1;
                    spellBook->motion.Vy = 0;
                    spellBook->motion.Vth = 0;
                }
            }
            /*if (rr.getDistance() < 0.4f)
            {
                cout << "ENTROU NA MERDA DO IF E A DISTANCIA É: " << rr.getDistance();
                spellBook->motion.Vth = 0;
                spellBook->motion.Vx = 0;
                spellBook->motion.Vy = 0;
                spellBook->motion.KickRight = true;
                
            }*/
            /*else
            {
                spellBook->motion.Vx = 0;
                spellBook->motion.Vy = 0;
                spellBook->motion.Vth = Deg2Rad(0.5f);
            }*/
        }
        else if (rr.getDistance() < 0.5f && !spellBook->perception.vision.ball.BallDetected)
        {
            contPerdido++;
            if(contPerdido < 10)
                spellBook->motion.HeadPitch = Deg2Rad(25);
            else
            {
                contPerdido = 0;
                scanPitch += Deg2Rad(20);
                if(scanPitch > spellBook->strategy.HeadPitchRange)
                    scanPitch = 0;
            }
            cout << "ScanPitch: " << scanPitch << endl;
            cout << "Quantas iterações: " << contPerdido << endl;
            spellBook->motion.Vx = 0.01f;
            spellBook->motion.Vy = 0;
            spellBook->motion.Vth = Deg2Rad(5) * SIG(rr.getYaw());
            cout << "Else IF distancia < 0.5" << endl;
        }
        else
        {
            cout << "se perdeu: " << endl;
            spellBook->motion.Vx = 0.01f;
            spellBook->motion.Vy = 0.02f;
            spellBook->motion.Vth = Deg2Rad(1.0f) * SIG(rr.getYaw());
        }
    }else{
        spellBook->motion.Vx = 1;
        spellBook->motion.Vy = 0;
        spellBook->motion.Vth = 0;
    }
}
