#include "GameController.h"

GameController::GameController(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    pressed = false;
}

void GameController::OnStart()
{
    gameState = INITIAL;
    spellBook->behaviour.Started = true;
}

void GameController::Tick(float ellapsedTime, SensorValues &sensor)
{
    if(sensor.sensors[Sensors::ChestBoard_Button] > 0)
    {
        pressed = true;
    }
    else if(pressed)
    {
        pressed = false;
        if(gameState == INITIAL)
            gameState = PENALIZED;
        else
        {
            if(gameState == PENALIZED)
                gameState = PLAYING;
            else
                gameState = PENALIZED;
        }
    }

    spellBook->strategy.Started = gameState != INITIAL;
    spellBook->strategy.Penalized = gameState == PENALIZED;
    spellBook->behaviour.Penalized =  gameState == PENALIZED;
}

void GameController::OnStop()
{
    
}

GameController::~GameController()
{
    
}