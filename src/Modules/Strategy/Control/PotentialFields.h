#ifndef _POTENTIAL_FIELDS_H_
#define _POTENTIAL_FIELDS_H_

#include "Core/Module.h"
#include "Eigen/Eigen"

class PotentialFields : public InnerModule
{
    private:
        Eigen::Vector2f Xo, Xr, Fatt, Frep, Ftot;
        float Katt, Krep, Eo, Kw;
        float Vmax, v, Wmax, w;
    public:
        PotentialFields(SpellBook *spellBook);
        ~PotentialFields();

        void Tick(float ellapsedTime);
};

#endif