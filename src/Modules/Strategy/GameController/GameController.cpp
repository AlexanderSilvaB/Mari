#include "GameController.h"
#include <boost/bind.hpp>
#include "Core/InitManager.h"

GameController::GameController(SpellBook *spellBook)
{
    touch = NULL;
    this->spellBook = spellBook;
    timeSincePress = 0;
}

void GameController::OnStart()
{
    boost::program_options::variables_map config = InitManager::GetBlackboard()->config;
    touch = (Touch *)new AgentTouch(config["player.team"].as<int>(), config["player.number"].as<int>());
    touch->readOptions(config); 
}

void GameController::Tick(float ellapsedTime)
{
    timeSincePress += ellapsedTime;
    //ButtonPresses button = touch->GetSensors();
    SensorValues sensor = touch->getSensors(kinematics);
    if(sensor.sensors[Sensors::ChestBoard_Button] > 0 && timeSincePress > 1.0f)
    {
        cout << "ChestBoard_Button" << endl;
        spellBook->strategySpell.Penalized = !spellBook->strategySpell.Penalized;
        timeSincePress = 0;
    }
}

void GameController::OnStop()
{
    delete touch;
}

GameController::~GameController()
{
    touch = NULL;
}