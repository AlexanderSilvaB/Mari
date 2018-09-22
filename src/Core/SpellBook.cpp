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
    perceptionSpell.Load(storage);
    remoteSpell.Load(storage);
}

void SpellBook::Save(string fileName)
{
    Storage storage(fileName);
    modulesSpell.Save(storage);
    motionSpell.Save(storage);
    perceptionSpell.Save(storage);
    remoteSpell.Save(storage);

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
    LoadStrategy = true;
    LoadMotion = true;
    LoadPerception = true;
    LoadRemote = true;
}

void ModulesSpell::Load(Storage &storage)
{
    LoadStrategy = storage["Modules"]["Strategy"]["Enabled"].Default(true);
    LoadMotion = storage["Modules"]["Motion"]["Enabled"].Default(true);
    LoadPerception = storage["Modules"]["Perception"]["Enabled"].Default(true);
    LoadRemote = storage["Modules"]["Remote"]["Enabled"].Default(true);
}

void ModulesSpell::Save(Storage &storage)
{
    storage["Modules"]["Strategy"]["Enabled"] = LoadStrategy;
    storage["Modules"]["Motion"]["Enabled"] = LoadMotion;
    storage["Modules"]["Perception"]["Enabled"] = LoadPerception;
    storage["Modules"]["Remote"]["Enabled"] = LoadRemote;
}

BallSpell::BallSpell()
{
    method = "CASCADE";
    ballWidth = ballHeight = 0.1f;

    BallAzimuth = 0;
    BallDetected = false;
    BallDistance = 0;
    BallElevation = 0;
}

void BallSpell::Load(Storage &storage)
{
    method = (string)storage["Modules"]["Perception"]["BallDetector"]["Ball"]["Method"].Default("CASCADE");
    ballWidth = storage["Modules"]["Perception"]["BallDetector"]["Ball"]["Size"][0].Default(0.1f);
    ballHeight = storage["Modules"]["Perception"]["BallDetector"]["Ball"]["Size"][1].Default(0.1f);
}

void BallSpell::Save(Storage &storage)
{
    storage["Modules"]["Perception"]["BallDetector"]["Ball"]["Method"] = method;
    storage["Modules"]["Perception"]["BallDetector"]["Ball"]["Size"][0] = ballWidth;
    storage["Modules"]["Perception"]["BallDetector"]["Ball"]["Size"][1] = ballHeight;
}

PerceptionSpell::PerceptionSpell()
{
    EnableBallDetector = true;
}

void PerceptionSpell::Load(Storage &storage)
{
    EnableBallDetector = storage["Modules"]["Perception"]["BallDetector"]["Enabled"].Default(true);
    ballSpell.Load(storage);
}

void PerceptionSpell::Save(Storage &storage)
{
    storage["Modules"]["Perception"]["BallDetector"]["Enabled"] = EnableBallDetector;
    ballSpell.Save(storage);
}

MotionSpell::MotionSpell()
{
    Remote = false;
    Stand = Stiff = false;
    Vx = Vy = Vth = 0;
    HeadYaw = HeadPitch = 0;
    HeadSpeed = 0.2f;
    HeadRelative = false;
    KickLeft = KickRight = false;
    LimpLeft = LimpRight = false;
    GetupBack = GetupFront = false;
}

void MotionSpell::Load(Storage &storage)
{
    
}

void MotionSpell::Save(Storage &storage)
{
    
}

RemoteSpell::RemoteSpell()
{
    EnableJoystick = true;
    EnableNetwork = true;
}

void RemoteSpell::Load(Storage &storage)
{
    EnableJoystick = storage["Modules"]["Remote"]["Joystick"]["Enabled"].Default(true);
    EnableNetwork = storage["Modules"]["Remote"]["Network"]["Enabled"].Default(true);
}

void RemoteSpell::Save(Storage &storage)
{
    storage["Modules"]["Remote"]["Joystick"]["Enabled"] = EnableJoystick;
    storage["Modules"]["Remote"]["Network"]["Enabled"] = EnableNetwork;
}