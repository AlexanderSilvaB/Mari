#include "SpellBook.h"
#include <iostream>

using namespace std;

SpellBook::SpellBook()
{
    pthread_mutex_init(&lock, NULL);
}

SpellBook::~SpellBook()
{
    pthread_mutex_destroy(&lock);
}

void SpellBook::Load(string fileName)
{
    Storage storage(fileName);
    modules.Load(storage);
    motion.Load(storage);
    perception.Load(storage);
    remote.Load(storage);
    strategy.Load(storage);
    behaviour.Load(storage);
}

void SpellBook::Save(string fileName)
{
    Storage storage(fileName);
    modules.Save(storage);
    motion.Save(storage);
    perception.Save(storage);
    remote.Save(storage);
    strategy.Save(storage);
    behaviour.Save(storage);

    storage.Save();
}

void SpellBook::Lock()
{
    pthread_mutex_lock(&lock);
}

void SpellBook::Unlock()
{
    pthread_mutex_unlock(&lock);
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
    LoadBehaviour = true;
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
    LoadBehaviour = storage["Modules"]["Behaviour"]["Enabled"].Default(true);
}

void ModulesSpell::Save(Storage &storage)
{
    storage["Modules"]["Strategy"]["Enabled"] = LoadStrategy;
    storage["Modules"]["Motion"]["Enabled"] = LoadMotion;
    storage["Modules"]["Perception"]["Enabled"] = LoadPerception;
    storage["Modules"]["Remote"]["Enabled"] = LoadRemote;
    storage["Modules"]["Behaviour"]["Enabled"] = LoadBehaviour;
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
    BallSpell *s = (BallSpell*)spell;
    COPY(s, method)
    COPY(s, ballWidth)
    COPY(s, ballHeight)
    COPY(s, BallAzimuth)
    COPY(s, BallDetected)
    COPY(s, BallDistance)
    COPY(s, BallElevation)
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
    PerceptionSpell *s = (PerceptionSpell*)spell;
    COPY(s, EnableBallDetector)
    ball.CopyTo(spell);
}

void PerceptionSpell::Load(Storage &storage)
{
    EnableBallDetector = storage["Modules"]["Perception"]["BallDetector"]["Enabled"].Default(true);
    ball.Load(storage);
}

void PerceptionSpell::Save(Storage &storage)
{
    storage["Modules"]["Perception"]["BallDetector"]["Enabled"] = EnableBallDetector;
    ball.Save(storage);
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
    MotionSpell *s = (MotionSpell*)spell;
    COPY(s, Remote)
    COPY(s, Stand)
    COPY(s, Stiff)
    COPY(s, Vx)
    COPY(s, Vy)
    COPY(s, Vth)
    COPY(s, HeadYaw)
    COPY(s, HeadPitch)
    COPY(s, HeadSpeed)
    COPY(s, HeadRelative)
    COPY(s, KickLeft)
    COPY(s, KickRight)
    COPY(s, LimpLeft)
    COPY(s, LimpRight)
    COPY(s, GetupBack)
    COPY(s, GetupFront)
    COPY(s, TipOver)
    COPY(s, Dead)
    COPY(s, Walk)
    COPY(s, Crouch)
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
    RemoteSpell *s = (RemoteSpell*)spell;
    COPY(s, EnableJoystick)
    COPY(s, EnableNetwork)
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
    StrategySpell *s = (StrategySpell*)spell;
    COPY(s, Started)
    COPY(s, Penalized)
    COPY(s, FallenBack)
    COPY(s, FallenFront)
    COPY(s, Die)
    COPY(s, TurnOver)
}

void StrategySpell::Load(Storage &storage)
{
    
}

void StrategySpell::Save(Storage &storage)
{
    
}

BehaviourSpell::BehaviourSpell()
{
    Started = false;
    Penalized = false;
    Fallen = false;
    Die = false;
}

void BehaviourSpell::CopyTo(Spell *spell)
{
    BehaviourSpell *s = (BehaviourSpell*)spell;
    COPY(s, Started)
    COPY(s, Penalized)
    COPY(s, Fallen)
    COPY(s, Die)
}

void BehaviourSpell::Load(Storage &storage)
{
    
}

void BehaviourSpell::Save(Storage &storage)
{
    
}