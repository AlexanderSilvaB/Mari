#ifndef LOCALIZER_H_
#define LOCALIZER_H_

#include "Core/Module.h"
#include "Core/SpellBook.h"

using namespace std;

class Localizer : public InnerModule
{
    public:
        Localizer(SpellBook *spellBook);
        void Tick(float ellapsedTime);
};

#endif
