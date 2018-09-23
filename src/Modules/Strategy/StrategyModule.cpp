#include "StrategyModule.h"
#include <boost/bind.hpp>
#include "Core/InitManager.h"

StrategyModule::StrategyModule(SpellBook *spellBook)
    : Module(spellBook, 20)
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

    spellBook->motionSpell.Dead = spellBook->strategySpell.Die;
    if(spellBook->strategySpell.Die)
        return;
    spellBook->motionSpell.TipOver = spellBook->strategySpell.TurnOver;
    if(spellBook->strategySpell.TurnOver)
        return;
    spellBook->motionSpell.GetupBack = spellBook->strategySpell.FallenBack;
    if(spellBook->strategySpell.FallenBack)
        return;
    spellBook->motionSpell.GetupFront = spellBook->strategySpell.FallenFront;
    if(spellBook->strategySpell.FallenFront)
        return;

    if(spellBook->strategySpell.Penalized)
    {
        spellBook->motionSpell.Stiff = true;
        spellBook->motionSpell.Stand = false;
        spellBook->motionSpell.Walk = false;
    }
    else
    {
        spellBook->motionSpell.Stiff = true;
        spellBook->motionSpell.Stand = true;
        spellBook->motionSpell.Walk = true;
        spellBook->motionSpell.Vx = 0.1f;
    }
}