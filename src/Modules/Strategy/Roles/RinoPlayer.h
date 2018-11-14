#ifndef RINO_PLAYER_H
#define RINO_PLAYER_H

#include "Role.h"

using namespace std;

class RinoPlayer: public Role {

private:

    bool onBall;
    bool onPosition;
    int contPerdido;
    float scanPitch;
    float time;
    int tique;
    int ultimoY;
    bool moonwalk;
public:

    RinoPlayer(SpellBook *spellBook);
    ~RinoPlayer();
    void Tick(float ellapsedTime, const SensorValues&);

};
#endif
