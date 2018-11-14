#ifndef DEFENDER_ROLE_H
#define DEFENDER_ROLE_H

#include "Role.h"

using namespace std;

class DefenderRole: public Role {

private:

    bool onBall;
    bool onPosition;
    int contPerdido;
    float scanPitch;
    int conta, conta2, conta3;
    float Deg, Vel;
    bool reset;
public:

    DefenderRole(SpellBook *spellBook);
    ~DefenderRole();
    void Tick(float ellapsedTime, const SensorValues&);

};
#endif
