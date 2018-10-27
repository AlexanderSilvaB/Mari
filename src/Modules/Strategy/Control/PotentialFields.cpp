#include "PotentialFields.h"
#include "Core/Utils/Math.h"

PotentialFields::PotentialFields(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    Xo.setZero();
    Xr.setZero();
    Fatt.setZero();
    Frep.setZero();
    Ftot.setZero();

    Eo = 0.25f;
    Katt = 0.1f;
    Krep = 0.1f;
    Kw = 0.1f;
    Vmax = 0.1f;
}

PotentialFields::~PotentialFields()
{
    
}

void PotentialFields::Tick(float ellapsedTime)
{
    if(spellBook->strategy.WalkForward || spellBook->strategy.WalkAside)
    {
        Xo(0) = spellBook->strategy.TargetX;
        Xo(1) = spellBook->strategy.TargetY;

        Xr(0) = spellBook->perception.vision.localization.X;
        Xr(1) = spellBook->perception.vision.localization.Y;

        /*
        Fatt = (Xo - Xr)*Katt;
        // Frep = 
        Ftot = Fatt + Frep;

        v = min(Ftot.norm(), Vmax);
        w = Kw * atan2(Ftot(1), Ftot(0)) - spellBook->perception.vision.localization.Theta;
        */

        float g = FixAngle(atan2(Fatt(1), Fatt(0)));
        float a = FixAngle(g - spellBook->perception.vision.localization.Theta);
        float b = FixAngle(spellBook->strategy.TargetTheta - g);

        float kp = 2.0f/11.0f;
        float ka = 345.0f/823.0f;
        float kb = -kp;

        Fatt = (Xo - Xr);
        v = min(Fatt.norm()*kp, Vmax);
        w = ka*a + kb*b;

        if(spellBook->strategy.WalkForward)
            spellBook->motion.Vx = v;
        else
            spellBook->motion.Vy = v;
        spellBook->motion.Vth = w;
    }
    else
    {
        spellBook->motion.Vx = 0;
        spellBook->motion.Vy = 0;
        spellBook->motion.Vth = 0;
    }
}