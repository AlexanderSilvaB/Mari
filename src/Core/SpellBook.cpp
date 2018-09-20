#include "SpellBook.h"

using namespace std;

SpellBook::SpellBook()
{

}

void SpellBook::Load(string fileName)
{
    Storage storage(fileName);
    modulesSpell.Load(storage);
    motionSpell.Load(storage);
    visionSpell.Load(storage);
}

void SpellBook::Save(string fileName)
{
    Storage storage(fileName);
    modulesSpell.Save(storage);
    motionSpell.Save(storage);
    visionSpell.Save(storage);

    storage.Save();
}

// Spell

Spell::Spell()
{

}

Spell::~Spell()
{

}

void Spell::Load(Storage &storage)
{

}

void Spell::Save(Storage &storage)
{

}

// Modules
ModulesSpell::ModulesSpell()
{
    LoadVision = true;
    LoadMotion = true;
    LoadPerception = true;
}

void ModulesSpell::Load(Storage &storage)
{
    LoadVision = storage["Modules"]["Vision"]["Enabled"].Default(true);
    LoadMotion = storage["Modules"]["Motion"]["Enabled"].Default(true);
    LoadPerception = storage["Modules"]["Perception"]["Enabled"].Default(true);
}

void ModulesSpell::Save(Storage &storage)
{
    storage["Modules"]["Vision"]["Enabled"] = LoadVision;
    storage["Modules"]["Motion"]["Enabled"] = LoadMotion;
    storage["Modules"]["Perception"]["Enabled"] = LoadPerception;
}

VisionSpell::VisionSpell()
{
    BallAzimuth = 0;
    BallDetected = false;
    BallDistance = 0;
    BallElevation = 0;
}

void VisionSpell::Load(Storage &storage)
{
    
}

void VisionSpell::Save(Storage &storage)
{
    
}

MotionSpell::MotionSpell()
{
    HeadAzimuth = 0;

    RemoteMotion = false;
    RemoteStand = RemoteStiff = false;
    RemoteVx = RemoteVy = RemoteVth = 0;
    RemoteHeadYaw = RemoteHeadPitch = 0;
    RemoteKickLeft = RemoteKickRight = false;
    RemoteLimpLeft = RemoteLimpRight = false;
}

void MotionSpell::Load(Storage &storage)
{
    
}

void MotionSpell::Save(Storage &storage)
{
    
}