#include "SpellBook.h"
#include <iostream>

using namespace std;

SpellBook::SpellBook()
{

}

void SpellBook::Load(string fileName)
{
    Storage storage(fileName);
    modules.Load(storage);
    motion.Load(storage);
    perception.Load(storage);
    remote.Load(storage);
    strategy.Load(storage);
}

void SpellBook::Save(string fileName)
{
    Storage storage(fileName);
    modules.Save(storage);
    motion.Save(storage);
    perception.Save(storage);
    remote.Save(storage);
    strategy.Save(storage);

    storage.Save();
}

// Spell

Spell::Spell()
{

}

Spell::~Spell()
{

}

void Spell::CopyTo(Spell *spell)
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

void ModulesSpell::CopyTo(Spell *spell)
{

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

void BallSpell::CopyTo(Spell *spell)
{

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

void PerceptionSpell::CopyTo(Spell *spell)
{

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
    TipOver = Dead = false;
    Walk = false;
    Crouch = false;
}

void MotionSpell::CopyTo(Spell *spell)
{

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

void RemoteSpell::CopyTo(Spell *spell)
{

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

StrategySpell::StrategySpell()
{
    Started = false;
    Penalized = false;
    FallenBack = false;
    FallenFront = false;
    Die = false;
    TurnOver = false;
}

void StrategySpell::CopyTo(Spell *spell)
{

}

void StrategySpell::Load(Storage &storage)
{
    
}

void StrategySpell::Save(Storage &storage)
{
    
}