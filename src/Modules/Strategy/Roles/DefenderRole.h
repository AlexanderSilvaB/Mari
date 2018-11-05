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
    int conta, conta2, conta3, kick;
    float Deg, Vel;
    bool kickLeft;
    bool reset;
public:

    DefenderRole(SpellBook *spellBook);
    ~DefenderRole();
    void Tick(float ellapsedTime, const SensorValues&);

};
#endif
