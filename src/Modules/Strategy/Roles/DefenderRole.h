#ifndef DEFENDER_ROLE_H
#define DEFENDER_ROLE_H

#include "Core/Module.h"
#include "Core/SpellBook.h"

using namespace std;

class DefenderRole: public InnerModule {

private:

    bool onBall;
    bool onPosition;
    int contPerdido;
    float scanPitch;

public:

    DefenderRole(SpellBook *spellBook);
    ~DefenderRole();
    void Tick(float ellapsedTime, const SensorValues&);

};
#endif
