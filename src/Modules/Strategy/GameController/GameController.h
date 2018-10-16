#ifndef _GameController_H_
#define _GameController_H_

#include "Core/SpellBook.h"
#include "types/SensorValues.hpp"

using namespace std;

class GameController
{
    public:
        enum GameStates { INITIAL, READY, SET, PENALIZED, PLAYING, FINISHED };
    private:
        bool pressed;
        SpellBook *spellBook;
        GameStates gameState;
    public:
        GameController(SpellBook *spellBook);
        void OnStart();
        void Tick(float ellapsedTime, SensorValues &sensor);
        void OnStop();
        ~GameController();
};

#endif