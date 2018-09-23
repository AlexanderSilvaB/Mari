#include "StrategyModule.h"

StrategyModule::StrategyModule(SpellBook *spellBook)
    : Module(spellBook, 20)
{
    gameController = new GameController(spellBook);   
}

StrategyModule::~StrategyModule()
{
    delete gameController;
}

void StrategyModule::OnStart()
{
    gameController->OnStart();
}

void StrategyModule::OnStop()
{
    gameController->OnStop();
}

void StrategyModule::Tick(float ellapsedTime)
{
    gameController->Tick(ellapsedTime);

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
    cout << "Strategy" << endl;
}