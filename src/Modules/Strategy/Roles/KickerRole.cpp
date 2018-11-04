#include "KickerRole.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CartesianCoord.h"

KickerRole::KickerRole(SpellBook *spellBook)
    : InnerModule(spellBook)
{
    onStart = false;
    kick = 0;
    kickLeft = false;
    searchState = 0;
}

KickerRole::~KickerRole()
{

}

void KickerRole::Tick(float ellapsedTime, const SensorValues &sensor)
{
    spellBook->strategy.MoveHead = false;
    spellBook->motion.HeadSpeedYaw = 0.2f;
    spellBook->motion.HeadSpeedPitch = 0.2f;
    
    if((spellBook->strategy.GameState == GC::READY || spellBook->strategy.GameState == GC::PLAYING) && !onStart)
    {
        if(spellBook->perception.vision.localization.Enabled)
        {
            spellBook->strategy.WalkForward = true;
            spellBook->strategy.TargetX = -1.0f;
            spellBook->strategy.TargetY = 0.0f;
            spellBook->strategy.TargetTheta = 0;

            CartesianCoord current(spellBook->perception.vision.localization.X, spellBook->perception.vision.localization.Y); 
            CartesianCoord desired(spellBook->strategy.TargetX, spellBook->strategy.TargetY); 
            if(current.distance(desired)  < 0.2f)
            {
                spellBook->strategy.WalkForward = false;
                onStart = true;
            }
        }
        else
        {
            onStart = true;
        }
    }
    if(spellBook->strategy.GameState == GC::PLAYING && onStart)
    {
        if(kick > 60)
        {
            searchState = 0;
            spellBook->motion.Vth = 0;
            spellBook->motion.Vx = 0;
            spellBook->motion.HeadPitch = 0;
            if(spellBook->strategy.FakeKick)
            {
                spellBook->motion.Vx = 0.3f;
            }
            else
            {
                spellBook->motion.KickLeft = kickLeft;
                spellBook->motion.KickRight = !kickLeft;
            }
            kick++;
            if(kick > 100)
            {
                kick = 0;
                if(spellBook->strategy.FakeKick)
                {
                    spellBook->motion.Vx = 0;
                }
                else
                {
                    spellBook->motion.KickLeft = false;
                    spellBook->motion.KickRight = false;
                }
            }
        }
        else
        {            
            spellBook->motion.KickLeft = false;
            spellBook->motion.KickRight = false;
            if(spellBook->perception.vision.ball.BallLostCount < 5)
            {
                searchState = 0;
                wait = 0;
                if(abs(spellBook->perception.vision.ball.BallYaw) > Deg2Rad(10.0f))
                {
                    spellBook->motion.Vth = -spellBook->perception.vision.ball.BallYaw * 0.5f;
                    spellBook->motion.Vx = 0;
                    kick = 0;
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
                        kick = 0;
                    }
                    else
                    {
                        spellBook->motion.Vx = 0;
                        if(spellBook->motion.HeadPitch > 0)
                        {
                            kick++;
                            kickLeft = spellBook->perception.vision.ball.BallYaw < 0;
                        }
                        else
                        {
                            kick = 0;
                        }
                    }
                }
            }
            else
            {
                kick = 0;

                // Avanço da máquina de estados
                wait++;
                if(wait >  100)
                {
                    searchState++;    
                    wait = 0;
                }
                
                // Controle da máquina de estados
                switch(searchState)
                {
                    case 0: // Só espera
                        lookingDown = false;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 1: // Procura no pé
                        lookingDown = true;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 2: // Procura na frente de novo
                        lookingDown = false;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 3: // Procura no lado esquerdo na frente
                        lookingDown = false;
                        turningLeft = true;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 4: // Procura no lado esquerdo no pé
                        lookingDown = true;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 5: // Procura no lado esquerdo na frente de novo
                        lookingDown = false;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 6: // Volta a olhar pra frente
                        lookingDown = false;
                        turningLeft = false;
                        turningRight = true;
                        goingForward = false;
                        break;
                    case 7: // Procura do lado direito
                        lookingDown = false;
                        turningLeft = false;
                        turningRight = true;
                        goingForward = false;
                        break;
                    case 8: // Procura do lado direito no pé
                        lookingDown = true;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 9: // Procura do lado direito na frente de novo
                        lookingDown = false;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 10: // Volta a olhar pra frente
                        lookingDown = false;
                        turningLeft = true;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 11: // Procura no pé
                        lookingDown = true;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        break;
                    case 12: // Anda pra frente
                    case 13:
                    case 14:
                        lookingDown = false;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = true;
                        break;
                    default: // Quando o ciclo terminar, começa de novo
                        lookingDown = false;
                        turningLeft = false;
                        turningRight = false;
                        goingForward = false;
                        searchState = 0;
                        break;
                }

                if(turningLeft)
                    spellBook->motion.Vth = -Deg2Rad(10.0f);
                else if(turningRight)
                    spellBook->motion.Vth = Deg2Rad(10.0f);
                else
                    spellBook->motion.Vth = 0.0f;
            
                if(goingForward)
                    spellBook->motion.Vx = 0.1f;
                else 
                    spellBook->motion.Vx = 0;

                if(lookingDown)
                    spellBook->motion.HeadPitch = Deg2Rad(24.0f);
                else
                    spellBook->motion.HeadPitch = 0.0f;
            }
        }
    }
}