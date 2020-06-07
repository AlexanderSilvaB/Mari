#include "Role.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CartesianCoord.h"

Role::Role(SpellBook *spellBook)
    : InnerModule(spellBook)
{
    kick = 0;
    kickLeft = false;
}

Role::~Role()
{

}

void Role::Tick(float ellapsedTime, const SensorValues &sensor)
{
    
}

void Role::CancelKick()
{
    kick = 0;
}

bool Role::Kicking()
{
    /*
    if(kick > 50)
    {
        kick++;
        if(spellBook->strategy.FakeKick)
        {
            spellBook->motion.Vth = 0;
            spellBook->motion.Vy = 0;
            spellBook->motion.Vx = 0.3f;
            if(kick > 100)
            {
                spellBook->motion.Vx = 0;
                kick = 0;
            }
        }
        else
        {
            spellBook->motion.Vx = 0.03f;
            if(kick < 60)
            {
                spellBook->motion.KickLeft = kickLeft;
                spellBook->motion.KickRight = !kickLeft;
            }
            else if(kick < 200)
            {
                spellBook->motion.KickLeft = false;
                spellBook->motion.KickRight = false;
            }
            else if(kick < 300)
            {
                spellBook->motion.KickLeft = kickLeft;
                spellBook->motion.KickRight = !kickLeft;
            }
            else
            {
                spellBook->motion.KickLeft = false;
                spellBook->motion.KickRight = false;
                kick = 0;
            }
        }
        return true;
    }
    */
    if(kick > 50)
    {
        kick++;
        spellBook->motion.Vx = spellBook->strategy.FakeKick ? 0.2f : 0.1f;
        spellBook->motion.Vth = 0;
        spellBook->motion.Vy = 0;
        if(kick > 170)
        {
            spellBook->motion.Vx = 0.0f;
            kick = 0;
        }
        else if(kick > 160)
        {
            spellBook->motion.KickLeft = false;
            spellBook->motion.KickRight = false;
        }
        else if(kick > 60 && !spellBook->strategy.FakeKick)
        {
            spellBook->motion.KickLeft = kickLeft;
            spellBook->motion.KickRight = !kickLeft;
        }
        return true;
    }
    return false;
}

bool Role::PrepareKick(float angle)
{
    kick++;
    kickLeft = angle < 0;
}