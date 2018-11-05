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
    scanPitch = 1;
    time = 0;
    tique = 0;
    moonwalk = false;
    spellBook->perception.vision.ball.BallYaw = 12;
}
RinoPlayer::~RinoPlayer()
{
}
void RinoPlayer::Tick(float ellapsedTime, const SensorValues &sensor)
{
    spellBook->strategy.MoveHead = false;
    float YawHead;
    CartesianCoord coord;
    RelativeCoord rr;
    if ((spellBook->strategy.GameState == STATE_READY || spellBook->strategy.GameState == STATE_PLAYING) &&
        !onPosition)
    {

        /*if (spellBook->perception.vision.localization.Enabled)
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
        }*/
        onPosition = true;
    }

    if (spellBook->strategy.GameState == STATE_PLAYING)
    {
        cout << "entrei em nada" << endl;
        spellBook->motion.KickRight = false;
        spellBook->motion.Vth = 0;
        spellBook->motion.Vx = 0;
        spellBook->motion.Vy = 0;

        if (!spellBook->perception.vision.ball.BallDetected)
        {
            cout << "Bola não detectada, situação do moonwalk: " << moonwalk << endl;
            spellBook->motion.HeadYaw = Deg2Rad(0);
            spellBook->motion.Vx = 0;
            spellBook->motion.Vy = 0;
            spellBook->motion.Vth = 0;
            spellBook->strategy.HeadYawRange = Deg2Rad(55.0f);
            spellBook->motion.HeadSpeedYaw = 0.02;
            spellBook->strategy.HeadPitchRange = Deg2Rad(35.0f);
            time += ellapsedTime;
            cout << " Não Detectando " << endl;
            // spellBook->perception.vision.ball.BallYaw
            cout << "A bola foi perdida mas o X é igual a: " << spellBook->perception.vision.ball.BallDistance << endl;

            spellBook->motion.HeadSpeedPitch = abs(0.0f);
            spellBook->motion.HeadPitch = scanPitch;
            scanPitch += Deg2Rad(0.4f);

            if (scanPitch > spellBook->strategy.HeadPitchRange)
            {
                scanPitch = Deg2Rad(-5.0f);
            }
            if (spellBook->perception.vision.ball.BallDistance > 0.1f && spellBook->perception.vision.ball.BallDistance < 0.5f && moonwalk == false)
            {
                spellBook->motion.Vx = 0;
                cout << "Era pra andar pra trás." << endl;
                if (time > 30 * ellapsedTime && time < 90 * ellapsedTime)
                {
                    spellBook->motion.Vx = -spellBook->perception.vision.ball.BallDistance / 2;
                    if (time > 88 * ellapsedTime)
                        moonwalk = true;
                }
                cout << "Andei para trás" << endl;
            }
            //if (time > 95 * ellapsedTime && time < 280 * ellapsedTime)
            //{
                if (spellBook->perception.vision.ball.BallYaw > 0)
                {
                    cout << "Time" << time << endl;
                    spellBook->motion.Vth = Deg2Rad(-30);
                }
                else
                {
                    spellBook->motion.Vth = Deg2Rad(30);
                }
            //}

            if (time > 200 * ellapsedTime && time < 320 * ellapsedTime) //Se o robo estiver rodando muito, aumentar o intervalo. Ou remover o IF e aumentar velocidade em X. Ou falar com o Pedro.
            {
                spellBook->motion.Vth = 0;
                spellBook->motion.Vx = 0.6f;
            }
        }
        else if (spellBook->perception.vision.ball.BallDetected)
        {
            time = 0;
            spellBook->motion.HeadSpeedPitch = abs(0.0f);
            moonwalk = false;
            spellBook->motion.Vth = 0;
            spellBook->motion.Vx = 0;
            spellBook->motion.Vy = 0;

            cout << "Bola detectada, situação do moonwalk: " << moonwalk << endl;

            if (spellBook->perception.vision.ball.BallYaw > Deg2Rad(5.0f))
            {
                cout << "ballY 5>" << endl;
                spellBook->strategy.MoveHead = false;
                spellBook->motion.Vth = -0.25 * (spellBook->perception.vision.ball.BallYaw);
                spellBook->motion.Vx = 0;
                spellBook->motion.Vy = 0;
            }
            else
            {
                cout << "alinhado" << endl;
                spellBook->strategy.MoveHead = false;
                spellBook->motion.Vth = 0.0f;
                spellBook->motion.Vx = 0.25 * (coord.getX() + 0.01);
                spellBook->motion.Vy = 0;
                //spellBook->motion.HeadYaw = 0;
                //spellBook->motion.HeadPitch = Deg2Rad(20.0f);
            }
        }
    }
}
