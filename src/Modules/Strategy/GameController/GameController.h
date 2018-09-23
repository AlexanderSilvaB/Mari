#ifndef _GameController_H_
#define _GameController_H_

#include "Core/SpellBook.h"
#include "Motion/rUNSWiftMotionAdapter.h"
#include "motion/touch/AgentTouch.hpp"

using namespace std;

class GameController
{
    private:
        float timeSincePress;
        Touch *touch;
        SpellBook *spellBook;
        Kinematics kinematics;
    public:
        GameController(SpellBook *spellBook);
        void OnStart();
        void Tick(float ellapsedTime);
        void OnStop();
        ~GameController();
};

#endif