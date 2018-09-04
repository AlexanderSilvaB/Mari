#include "RemoteModule.h"

RemoteModule::RemoteModule(SpellBook *spellBook) : Module(spellBook, 100)
{
    spellBook->motionSpell.RemoteMotion = false;
    joystick = new Joystick("/dev/hidraw0");
    usingJoystick = false;
}

RemoteModule::~RemoteModule()
{
    delete joystick;
}

void RemoteModule::Tick(float ellapsedTime)
{
    if (joystick != NULL && joystick->isFound())
    {
        if(usingJoystick == false)
        {
            cout << "Using joystick" << endl;
            usingJoystick = true;
        }
        JoystickEvent event;
        if (joystick->sample(&event))
        {
            if (event.isAxis())
            {
                cout << "Axis " << event.number << " is at position " << event.value << endl;
            }
        }
    }
    else
    {
        if(usingJoystick == true)
        {
            cout << "Not using joystick" << endl;
            usingJoystick = false;
        }
    }
}