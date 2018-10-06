#include "BehaviourModule.h"
#include <boost/bind.hpp>
#include "Core/InitManager.h"
#include "utils/speech.hpp"

BehaviourModule::BehaviourModule(SpellBook *spellBook)
    : Module(spellBook, "Behaviour", 500)
{
    timeSinceAct = 0;
}

BehaviourModule::~BehaviourModule()
{

}

void BehaviourModule::OnStart()
{
    
}

void BehaviourModule::OnStop()
{
    
}

void BehaviourModule::Tick(float ellapsedTime)
{
    timeSinceAct += ellapsedTime;

    if(timeSinceAct > 2.0f)
    {
        if(spellBook->behaviour.Fallen)
        {
            spellBook->behaviour.Fallen = false;
            SAY("The bakery fell");
        }
        else if(spellBook->behaviour.Started)
        {
            spellBook->behaviour.Started = false;
            SAY("Can wait to kill some players");
        }
        else if(spellBook->behaviour.Penalized)
        {
            SAY("Let me play, now");
        }
        else if(spellBook->behaviour.Die)
        {
            spellBook->behaviour.Die = false;
            SAY("I am dead");
        }
    }
}