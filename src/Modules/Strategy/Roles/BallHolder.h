#ifndef BALL_HOLDER_H
#define BALL_HOLDER_H

#include "Role.h"

using namespace std;

class BallHolder: public Role {

private:

    bool onBall;
    bool onPosition;
    float scanPitch;
    int conta, conta2;

public:

    BallHolder(SpellBook *spellBook);
    ~BallHolder();
    void Tick(float ellapsedTime, const SensorValues&);

};
#endif