#ifndef _P_CONTROL_H_
#define _P_CONTROL_H_

#include "Core/Module.h"
#include "Eigen/Eigen"

class PControl : public InnerModule
{
    private:
        Eigen::Vector3f Xo, Xr, F;
        float kp, kb, ka;
        float Vmax, v, Wmax, w;
    public:
        PControl(SpellBook *spellBook);
        ~PControl();

        void Tick(float ellapsedTime);
};

#endif