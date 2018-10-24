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

void BehaviourModule::Load()
{
    LOAD(behaviour);
    LOAD(perception);
}

void BehaviourModule::Save()
{
    SAVE(behaviour);
}


void BehaviourModule::Tick(float ellapsedTime)
{
    timeSinceAct += ellapsedTime;

    if(spellBook->behaviour.Fallen && timeSinceAct > 2.0f)
    {
        spellBook->behaviour.Fallen = false;
        timeSinceAct = 0;
        spellBook->behaviour.LeftEye = BLUE;
        spellBook->behaviour.RightEye = BLUE;
        SAY("The bakery fell");
    }
    else if(spellBook->behaviour.Started && timeSinceAct > 2.0f)
    {
        spellBook->behaviour.Started = false;
        timeSinceAct = 0;
        spellBook->behaviour.LeftEye = RED;
        spellBook->behaviour.RightEye = RED;
        SAY("Can wait to kill some players");
    }
    else if(spellBook->behaviour.Penalized && timeSinceAct > 10.0f)
    {
        timeSinceAct = 0;
        spellBook->behaviour.LeftEye = RED;
        spellBook->behaviour.RightEye = BLACK;
        SAY("Let me play, now");
    }
    else if(spellBook->behaviour.Die && timeSinceAct > 2.0f)
    {
        spellBook->behaviour.Die = false;
        timeSinceAct = 0;
        spellBook->behaviour.LeftEye = BLACK;
        spellBook->behaviour.RightEye = BLACK;
        SAY("I am dead");
    }
    else
    {
        if(spellBook->perception.vision.ball.BallDetected)
            spellBook->behaviour.LeftEye = BLUE;
        else
            spellBook->behaviour.LeftEye = GREEN;
        spellBook->behaviour.RightEye = RED;
    }
}