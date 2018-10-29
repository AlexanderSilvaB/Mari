#ifndef _HEAD_CONTROLLER_H
#define _HEAD_CONTROLLER_H

#include "Core/Module.h"
#include "Core/Utils/CartesianCoord.h"
#include "types/SensorValues.hpp"

class HeadController : public InnerModule
{
    private:
        bool flip;
        float closeInPitch;
        float farPitch;

        bool ballCloseIn;
        float ballCloseInDistance;

        int numFramesTracked;
        float calculateDesiredPitch(CartesianCoord &neckRelative);
        float calculateDesiredYaw(CartesianCoord &neckRelativeTarget);
    public:
        HeadController(SpellBook *spellBook);
        ~HeadController();

        void Tick(float ellapsedTime, const SensorValues &sensor);
};

#endif
