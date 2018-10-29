#include "GameController.h"

GameController::GameController(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    pressed = false;
}

void GameController::OnStart()
{
    spellBook->behaviour.Started = true;
}

void GameController::Tick(float ellapsedTime, const SensorValues &sensor)
{
    if(sensor.sensors[Sensors::ChestBoard_Button] > 0)
    {
        pressed = true;
    }
    else if(pressed)
    {
        pressed = false;
        if(spellBook->strategy.GameState == GC::INITIAL)
            spellBook->strategy.GameState = GC::PENALIZED;
        else
        {
            if(spellBook->strategy.GameState == GC::PENALIZED)
                spellBook->strategy.GameState = GC::PLAYING;
            else
                spellBook->strategy.GameState = GC::PENALIZED;
        }
    }

    spellBook->strategy.Started = spellBook->strategy.GameState != GC::INITIAL;
    spellBook->strategy.Penalized = spellBook->strategy.GameState == GC::PENALIZED;
    spellBook->behaviour.Penalized =  spellBook->strategy.GameState == GC::PENALIZED;
}

void GameController::OnStop()
{
    
}

GameController::~GameController()
{
    
}