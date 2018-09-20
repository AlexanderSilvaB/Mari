#ifndef _REMOTEMODULE_H_
#define _REMOTEMODULE_H_

#include "Core/Module.h"
#include "Joystick.h"

using namespace std;

class RemoteModule : public Module
{
    private:
        Joystick *joystick;
        bool usingJoystick;
    public:
        RemoteModule(SpellBook *spellBook);
        ~RemoteModule();
        void Tick(float ellapsedTime);
};

#endif
