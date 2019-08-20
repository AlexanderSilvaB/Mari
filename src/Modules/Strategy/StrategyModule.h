#ifndef _StrategyModule_H_
#define _StrategyModule_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

#include "motion/touch/AgentTouch.hpp"
#include "GameController/GameController.h"
#include "Safety/SafetyMonitor.h"

#include "Control/PControl.h"
#include "Control/PotentialFields.h"
#include "Control/HeadController.h"

#include "Trackers/BallTracker.h"
#include "Trackers/RobotTracker.h"
#include "Trackers/FeatureTracker.h"

#include "Roles/GoalieRole.h"
#include "Roles/DefenderRole.h"
#include "Roles/KickerRole.h"
#include "Roles/RinoPlayer.h"
#include "Roles/BallHolder.h"
#include "Roles/LocalizerRole.h"

using namespace std;

class StrategyModule : public Module
{
    private:
        GameController *gameController;
        SafetyMonitor *safetyMonitor;

        PotentialFields *potentialFields;
        PControl *pControl;

        HeadController *headController;

        BallTracker *ballTracker;
        RobotTracker *robotTracker;
        FeatureTracker *featureTracker;

        // Roles
        GoalieRole *goalie;
        DefenderRole *defender;
        KickerRole *kicker;
        RinoPlayer *rPlayer;
        BallHolder *ballHolder;
        LocalizerRole *loca;


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