#ifndef _BehaviourModule_H_
#define _BehaviourModule_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

class BehaviourModule : public Module
{
    private:
        float timeSinceAct;
    public:
        BehaviourModule(SpellBook *spellBook);
        ~BehaviourModule();
        void Tick(float ellapsedTime);
        void OnStart();
        void OnStop();
};

#endif