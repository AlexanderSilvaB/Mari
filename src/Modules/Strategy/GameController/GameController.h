#ifndef _GameController_H_
#define _GameController_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"
#include "types/SensorValues.hpp"
#include "types/ButtonPresses.hpp"
#include "gamecontroller/RoboCupGameControlData.hpp"

using namespace std;

class GameController : public InnerModule
{
    private:
        bool pressed;
    public:
        GameController(SpellBook *spellBook);
        void OnStart();
        void Tick(float ellapsedTime, const SensorValues &sensor);
        void OnStop();
        ~GameController();
};

#endif