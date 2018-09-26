#include "StrategyModule.h"
#include <boost/bind.hpp>
#include "Core/InitManager.h"

StrategyModule::StrategyModule(SpellBook *spellBook)
    : Module(spellBook, "Strategy", 20)
{
    touch = NULL;
    gameController = new GameController(spellBook);   
    safetyMonitor = new SafetyMonitor(spellBook);
}

StrategyModule::~StrategyModule()
{
    delete gameController;
    delete safetyMonitor;
    touch = NULL;
}

void StrategyModule::OnStart()
{
    boost::program_options::variables_map config = InitManager::GetBlackboard()->config;
    touch = (Touch *)new AgentTouch(config["player.team"].as<int>(), config["player.number"].as<int>());
    touch->readOptions(config); 
    
    gameController->OnStart();
    safetyMonitor->OnStart();
}

void StrategyModule::OnStop()
{
    gameController->OnStop();
    safetyMonitor->OnStop();
    delete touch;
}

void StrategyModule::Tick(float ellapsedTime)
{
    SensorValues sensor = touch->getSensors(kinematics);

    safetyMonitor->Tick(ellapsedTime, sensor);
    gameController->Tick(ellapsedTime, sensor);

    if(!spellBook->strategy.Started)
    {
        spellBook->motion.Stiff = false;
        spellBook->motion.Stand = false;
        spellBook->motion.Walk = false;
        return;
    }

    if(spellBook->strategy.Penalized)
    {
        spellBook->motion.Stiff = true;
        spellBook->motion.Stand = false;
        spellBook->motion.Walk = false;
        return;
    }

    spellBook->motion.Dead = spellBook->strategy.Die;
    if(spellBook->strategy.Die)
        return;
    spellBook->motion.TipOver = spellBook->strategy.TurnOver;
    if(spellBook->strategy.TurnOver)
        return;
    spellBook->motion.GetupBack = spellBook->strategy.FallenBack;
    if(spellBook->strategy.FallenBack)
        return;
    spellBook->motion.GetupFront = spellBook->strategy.FallenFront;
    if(spellBook->strategy.FallenFront)
        return;

    spellBook->motion.Stiff = true;
    spellBook->motion.Stand = true;
    spellBook->motion.Walk = true;
    spellBook->motion.Vx = 0.3f;
}