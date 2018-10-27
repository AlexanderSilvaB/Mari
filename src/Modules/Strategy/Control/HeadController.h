#ifndef _HEAD_CONTROLLER_H
#define _HEAD_CONTROLLER_H

#include "Core/Module.h"

class HeadController : public InnerModule
{
    private:
        float closeInPitch;
        float farPitch;

        bool ballCloseIn;
        float ballCloseInDistance;

        int numFramesTracked;
        float calculateDesiredPitch(XYZ_Coord neckRelative);
        float calculateDesiredYaw(XYZ_Coord neckRelativeTarget);
    public:
        HeadController(SpellBook *spellBook);
        ~HeadController();

        void Tick(float ellapsedTime);
};

#endif
