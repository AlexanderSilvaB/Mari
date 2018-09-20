#ifndef _PERCEPTIONMODULE_H_
#define _PERCEPTIONMODULE_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

#ifdef USE_UNSW
#include "rUNSWiftPerceptionAdapter.h"
#endif

using namespace std;

class PerceptionModule : public Module
{
    private:
        #ifdef USE_UNSW
        rUNSWiftPerceptionAdapter *perception;
        #endif
    public:
        PerceptionModule(SpellBook *spellBook);
        ~PerceptionModule();
        void Tick(float ellapsedTime);
        void OnStart();
        void OnStop();
};

#endif