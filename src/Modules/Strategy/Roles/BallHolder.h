#ifndef BALL_HOLDER_H
#define BALL_HOLDER_H

#include "Core/Module.h"
#include "Core/SpellBook.h"

using namespace std;

class BallHolder: public InnerModule {

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