#include "GameController.h"

GameController::GameController(SpellBook *spellBook)
{
    this->spellBook = spellBook;
    timeSincePress = 0;
}

void GameController::OnStart()
{
    
}

void GameController::Tick(float ellapsedTime, SensorValues &sensor)
{
    timeSincePress += ellapsedTime;
    if(sensor.sensors[Sensors::ChestBoard_Button] > 0 && timeSincePress > 1.0f)
    {
        cout << "ChestBoard_Button" << endl;
        spellBook->strategy.Penalized = !spellBook->strategy.Penalized;
        timeSincePress = 0;
    }
}

void GameController::OnStop()
{
    
}

GameController::~GameController()
{
    
}