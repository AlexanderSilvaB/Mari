#ifndef _StrategyModule_H_
#define _StrategyModule_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

#include "motion/touch/AgentTouch.hpp"
#include "GameController/GameController.h"
#include "Safety/SafetyMonitor.h"

using namespace std;

class StrategyModule : public Module
{
    private:
        Touch *touch;
        Kinematics kinematics;

        GameController *gameController;
        SafetyMonitor *safetyMonitor;

        int squareStep;
        float squareX, squareY;
        float squareL;
        float squareTimer;
        float circleRadius;
    public:
        StrategyModule(SpellBook *spellBook);
        ~StrategyModule();
        void Tick(float ellapsedTime);
        void OnStart();
        void OnStop();
        void Load();
        void Save();
};

#endif