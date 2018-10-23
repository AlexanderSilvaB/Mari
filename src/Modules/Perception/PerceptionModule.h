#ifndef _PERCEPTIONMODULE_H_
#define _PERCEPTIONMODULE_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

#include "rUNSWiftPerceptionAdapter.h"

#include "Vision.h"

using namespace std;

class PerceptionModule : public Module
{
    private:
        rUNSWiftPerceptionAdapter *perception;
        Vision *vision;
    public:
        PerceptionModule(SpellBook *spellBook);
        ~PerceptionModule();
        void Tick(float ellapsedTime);
        void OnStart();
        void OnStop();
        void Load();
        void Save();
};

#endif