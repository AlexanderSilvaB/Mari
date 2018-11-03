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
    float YawHead;
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
        cout << "entrei em nada" << endl;
        spellBook->motion.KickRight = false;
        spellBook->motion.Vth = 0;
        spellBook->motion.Vx = 0;
        spellBook->motion.Vy = 0;
        
        if (!spellBook->perception.vision.ball.BallDetected)
        {
            spellBook->strategy.MoveHead = true;            
            spellBook->motion.Vx = 0;
            spellBook->motion.Vy = 0;
            spellBook->motion.Vth = 0;
            spellBook->strategy.HeadYawRange = Deg2Rad(55.0f);
            spellBook->motion.HeadSpeedYaw = 0.02;
            time+=ellapsedTime;
            cout << " Não Detectando " << endl;
            
            if (time > 240 * ellapsedTime && time< 320*ellapsedTime)            
            {
                spellBook->strategy.MoveHead = false;
                cout << "Time" << time << endl;
                spellBook->motion.Vth = Deg2Rad(5.0f);
            }
        }
        else if(spellBook->perception.vision.ball.BallDetected)
        {
            time = 0;
            
            if(spellBook->perception.vision.ball.BallYaw > Deg2Rad(5.0f))
            {
                cout<<"ballY 5>"<<endl;
                spellBook->strategy.MoveHead = false;
                spellBook->motion.Vth = -0.25 * (spellBook->perception.vision.ball.BallYaw);
                spellBook->motion.Vx = 0;
                spellBook->motion.Vy = 0;
            }
            else
            {
                cout<<"alinhado"<<endl;
                spellBook->strategy.MoveHead = false;
                spellBook->motion.Vth = 0.0f;
                spellBook->motion.Vx = 0.25*coord.getX() + 0.1;
                spellBook->motion.Vy = 0;
                //spellBook->motion.HeadYaw = 0;
                //spellBook->motion.HeadPitch = Deg2Rad(20.0f);
            }
        }
    }
}
