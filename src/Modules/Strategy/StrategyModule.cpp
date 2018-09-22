#include "StrategyModule.h"

StrategyModule::StrategyModule(SpellBook *spellBook)
    : Module(spellBook, 50)
{
    
}

StrategyModule::~StrategyModule()
{
    
}

void StrategyModule::OnStart()
{
    
}

void StrategyModule::OnStop()
{
    
}

void StrategyModule::Tick(float ellapsedTime)
{
    spellBook->motionSpell.Stiff = true;
    spellBook->motionSpell.Stand = true;
    spellBook->motionSpell.Vx = 0.3f;
}