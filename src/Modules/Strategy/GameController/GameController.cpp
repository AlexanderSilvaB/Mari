#include "GameController.h"
#include "Core/GC/RoboCupGameControlData.h"

GameController::GameController(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    pressed = false;
    timeSincePressed = 0;
}

void GameController::OnStart()
{
    spellBook->behaviour.Started = true;
}

void GameController::Tick(float ellapsedTime, const SensorValues &sensor)
{
    timeSincePressed += ellapsedTime;

    if(timeSincePressed > 0.2f)
    {
        if(sensor.sensors[Sensors::ChestBoard_Button] > 0)
        {
            pressed = true;
        }
        else if(pressed)
        {
            timeSincePressed = 0;
            pressed = false;
            if(spellBook->strategy.GameState == STATE_INITIAL)
                spellBook->strategy.GameState = STATE_PENALISED;
            else
            {
                if(spellBook->strategy.GameState == STATE_PENALISED)
                    spellBook->strategy.GameState = STATE_PLAYING;
                else
                    spellBook->strategy.GameState = STATE_PENALISED;
            }
        }
    }

    spellBook->strategy.Started = spellBook->strategy.GameState != STATE_INITIAL;
    spellBook->strategy.Penalized = spellBook->strategy.GameState == STATE_PENALISED;
    spellBook->behaviour.Penalized =  spellBook->strategy.GameState == STATE_PENALISED;
}

void GameController::OnStop()
{
    
}

GameController::~GameController()
{
    
}