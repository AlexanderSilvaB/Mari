#ifndef _ROLES_KICKER_H_
#define _ROLES_KICKER_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"

using namespace std;

class KickerRole : public InnerModule
{
    private:
        bool onStart;
        int kick;
        bool kickLeft;
        bool preKick;
        int wait;
        int searchState;
        bool lookingDown, turningLeft, turningRight, goingForward;
    public:
        KickerRole(SpellBook *spellBook);
        void Tick(float ellapsedTime, const SensorValues &sensor);
        ~KickerRole();
};

#endif