#ifndef _StrategyModule_H_
#define _StrategyModule_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

using namespace std;

class StrategyModule : public Module
{
    private:
        
    public:
        StrategyModule(SpellBook *spellBook);
        ~StrategyModule();
        void Tick(float ellapsedTime);
        void OnStart();
        void OnStop();
};

#endif