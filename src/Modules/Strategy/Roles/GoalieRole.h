#ifndef _ROLES_GOALIE_H_
#define _ROLES_GOALIE_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"

using namespace std;

class GoalieRole : public InnerModule
{
    private:
        bool onGoal;
    public:
        GoalieRole(SpellBook *spellBook);
        void Tick(float ellapsedTime, const SensorValues &sensor);
        ~GoalieRole();
};

#endif