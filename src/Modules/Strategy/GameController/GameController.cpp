#include "GameController.h"

GameController::GameController(SpellBook *spellBook)
{
    this->spellBook = spellBook;
    timeSincePress = 0;
}

void GameController::OnStart()
{
    gameState = INITIAL;
    spellBook->behaviour.Started = true;
}

void GameController::Tick(float ellapsedTime, SensorValues &sensor)
{
    timeSincePress += ellapsedTime;
    if(sensor.sensors[Sensors::ChestBoard_Button] > 0 && timeSincePress > 0.2f)
    {
        if(gameState == INITIAL)
            gameState = PENALIZED;
        else
        {
            if(gameState == PENALIZED)
                gameState = PLAYING;
            else
                gameState = PENALIZED;
        }
        timeSincePress = 0;
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