#ifndef _SAFETY_MONITOR_H_
#define _SAFETY_MONITOR_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"
#include "types/SensorValues.hpp"

using namespace std;

#define MIN_STANDING_WEIGHT 0.55f
#define FALLEN 9
#define FALLING 8
#define FALLEN_ANG 70
#define FALLING_ANG 50 

class SafetyMonitor : public InnerModule
{
    public:
        SafetyMonitor(SpellBook *spellBook);
        void OnStart();
        void Tick(float ellapsedTime, const SensorValues &sensor);
        void OnStop();
        ~SafetyMonitor();
};

#endif