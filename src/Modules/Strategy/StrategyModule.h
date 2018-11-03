#ifndef _StrategyModule_H_
#define _StrategyModule_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

#include "motion/touch/AgentTouch.hpp"
#include "GameController/GameController.h"
#include "Safety/SafetyMonitor.h"
#include "Control/PotentialFields.h"
#include "Control/HeadController.h"
#include "Trackers/BallTracker.h"
#include "Roles/GoalieRole.h"
//#include "Roles/DefenderRole.h"
#include "Roles/RinoPlayer.h"

using namespace std;

class StrategyModule : public Module
{
    private:
        GameController *gameController;
        SafetyMonitor *safetyMonitor;
        PotentialFields *potentialFields;
        HeadController *headController;

        BallTracker *ballTracker;

        // Roles
        GoalieRole *goalie;
        //DefenderRole *defender;
        RinoPlayer *rinoplayer;

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