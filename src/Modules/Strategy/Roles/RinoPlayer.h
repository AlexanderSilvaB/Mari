#ifndef RINO_PLAYER_H
#define RINO_PLAYER_H

#include "Core/Module.h"
#include "Core/SpellBook.h"

using namespace std;

class RinoPlayer: public InnerModule {

private:

    bool onBall;
    bool onPosition;
    int contPerdido;
    float scanPitch;
    float time;
public:

    RinoPlayer(SpellBook *spellBook);
    ~RinoPlayer();
    void Tick(float ellapsedTime, const SensorValues&);

};
#endif
