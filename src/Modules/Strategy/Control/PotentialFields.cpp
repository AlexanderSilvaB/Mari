#include "PotentialFields.h"

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
    Kw = 0.7f;
    Vmax = 0.3f;
    file.open("localization.txt");
}

PotentialFields::~PotentialFields()
{
    file.close();
}

void PotentialFields::Tick(float ellapsedTime)
{
    file << spellBook->perception.vision.localization.X << "; " << spellBook->perception.vision.localization.Y << endl;
    if(spellBook->strategy.WalkForward || spellBook->strategy.WalkAside)
    {
        Xo(0) = spellBook->strategy.TargetX;
        Xo(1) = spellBook->strategy.TargetY;

        Xr(0) = spellBook->perception.vision.localization.X;
        Xr(1) = spellBook->perception.vision.localization.Y;

        Fatt = (Xo - Xr)*Katt;
        // Frep = 
        Ftot = Fatt + Frep;

        v = min(Ftot.norm(), Vmax);
        w = Kw * atan2(Ftot(1), Ftot(0)) - spellBook->perception.vision.localization.Theta;

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