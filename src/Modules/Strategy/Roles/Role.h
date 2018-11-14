#ifndef _ROLE_H_
#define _ROLE_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"

class Role : public InnerModule 
{
    private:
        int kick;
        bool kickLeft;
    protected:
        bool Kicking();
        void CancelKick();
        bool PrepareKick(float angle);
    public:
        Role(SpellBook *spellBook);
        virtual void Tick(float ellapsedTime, const SensorValues &sensor);
        virtual ~Role();
};

#endif
