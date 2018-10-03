#include "BehaviourModule.h"
#include <boost/bind.hpp>
#include "Core/InitManager.h"
#include "utils/speech.hpp"

BehaviourModule::BehaviourModule(SpellBook *spellBook)
    : Module(spellBook, "Behaviour", 500)
{
    
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
    if(spellBook->behaviour.Fallen)
    {
        spellBook->behaviour.Fallen = false;
        SAY("O forninho caiu");
    }
}