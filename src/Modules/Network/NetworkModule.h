#ifndef _NETWORKMODULE_H_
#define _NETWORKMODULE_H_

#include "Core/Module.h"

using namespace std;

class NetworkModule : public Module
{
    private:
        
    public:
        NetworkModule(SpellBook *spellBook);
        ~NetworkModule();
        void Tick(float ellapsedTime);
};

#endif
