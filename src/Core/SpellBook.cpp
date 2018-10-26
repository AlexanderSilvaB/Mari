#include "SpellBook.h"
#include "InitManager.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;
namespace po = boost::program_options;

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

void SpellBook::AddOptions(po::options_description &description)
{
    modules.AddOptions(description);
    motion.AddOptions(description);
    perception.AddOptions(description);
    remote.AddOptions(description);
    strategy.AddOptions(description);
    behaviour.AddOptions(description);    
}

void SpellBook::Update()
{
    po::variables_map config = InitManager::GetBlackboard()->config;
    modules.Update(config);
    motion.Update(config);
    perception.Update(config);
    remote.Update(config);
    strategy.Update(config);
    behaviour.Update(config);
}

void SpellBook::Lock()
{
    //pthread_mutex_lock(&lock);
}

void SpellBook::Unlock()
{
    //pthread_mutex_unlock(&lock);
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

void Spell::AddOptions(po::options_description &description)
{

}

void Spell::Update(const po::variables_map &config)
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
    LoadNetwork = true;
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
    LoadNetwork = storage["Modules"]["Network"]["Enabled"].Default(true);
}

void ModulesSpell::Save(Storage &storage)
{
    storage["Modules"]["Strategy"]["Enabled"] = LoadStrategy;
    storage["Modules"]["Motion"]["Enabled"] = LoadMotion;
    storage["Modules"]["Perception"]["Enabled"] = LoadPerception;
    storage["Modules"]["Remote"]["Enabled"] = LoadRemote;
    storage["Modules"]["Behaviour"]["Enabled"] = LoadBehaviour;
    storage["Modules"]["Network"]["Enabled"] = LoadNetwork;
}

BallSpell::BallSpell()
{
    Enabled = true;
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
    COPY(s, Enabled)
    COPY(s, method)
    COPY(s, ballWidth)
    COPY(s, ballHeight)
    COPY(s, BallAzimuth)
    COPY(s, HeadSpeed)
    COPY(s, BallDetected)
    COPY(s, BallDistance)
    COPY(s, BallElevation)
}

void BallSpell::Load(Storage &storage)
{
    Enabled = storage["Modules"]["Perception"]["Vision"]["BallDetector"]["Enabled"].Default(true);
    method = (string)storage["Modules"]["Perception"]["Vision"]["BallDetector"]["Method"].Default("CASCADE");
    ballWidth = storage["Modules"]["Perception"]["Vision"]["BallDetector"]["Ball"]["Size"][0].Default(0.1f);
    ballHeight = storage["Modules"]["Perception"]["Vision"]["BallDetector"]["Ball"]["Size"][1].Default(0.1f);
}

void BallSpell::Save(Storage &storage)
{
    storage["Modules"]["Perception"]["Vision"]["BallDetector"]["Enabled"] = Enabled;
    storage["Modules"]["Perception"]["Vision"]["BallDetector"]["Method"] = method;
    storage["Modules"]["Perception"]["Vision"]["BallDetector"]["Ball"]["Size"][0] = ballWidth;
    storage["Modules"]["Perception"]["Vision"]["BallDetector"]["Ball"]["Size"][1] = ballHeight;
}

LocalizationSpell::LocalizationSpell()
{
    Enabled = true;

    X = 0;
    Y = 0;
    Theta = 0;
}

void LocalizationSpell::CopyTo(Spell *spell)
{
    LocalizationSpell *s = (LocalizationSpell*)spell;
    COPY(s, Enabled)
    COPY(s, X)
    COPY(s, Y)
    COPY(s, Theta)
}

void LocalizationSpell::Load(Storage &storage)
{
    Enabled = storage["Modules"]["Perception"]["Vision"]["Localization"]["Enabled"].Default(true);
    
    X = storage["Modules"]["Perception"]["Vision"]["Localization"]["Position"][0].Default(0.0f);
    Y = storage["Modules"]["Perception"]["Vision"]["Localization"]["Position"][1].Default(0.0f);
    Theta = storage["Modules"]["Perception"]["Vision"]["Localization"]["Position"][2].Default(0.0f);
}

void LocalizationSpell::Save(Storage &storage)
{
    storage["Modules"]["Perception"]["Vision"]["Localization"]["Enabled"] = Enabled;
}

VisionSpell::VisionSpell()
{
    Enabled = true;
    BGR = false;
    HSV = false;
    GRAY = false;  
}

void VisionSpell::CopyTo(Spell *spell)
{
    VisionSpell *s = (VisionSpell*)spell;
    ball.CopyTo(&(s->ball));
    localization.CopyTo(&(s->localization));
    COPY(s, Enabled)
    COPY(s, BGR)
    COPY(s, HSV)
    COPY(s, GRAY)
}

void VisionSpell::Load(Storage &storage)
{
    Enabled = storage["Modules"]["Perception"]["Vision"]["Localization"]["Enabled"].Default(true);
    ball.Load(storage);
    localization.Load(storage);
}

void VisionSpell::Save(Storage &storage)
{
    ball.Save(storage);
    localization.Save(storage);
    storage["Modules"]["Perception"]["Vision"]["Localization"]["Enabled"] = Enabled;
}

PerceptionSpell::PerceptionSpell()
{
    
}

void PerceptionSpell::CopyTo(Spell *spell)
{
    PerceptionSpell *s = (PerceptionSpell*)spell;
    vision.CopyTo(&(s->vision));
}

void PerceptionSpell::Load(Storage &storage)
{
    vision.Load(storage);
}

void PerceptionSpell::Save(Storage &storage)
{
    vision.Save(storage);
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

    GoalieSit = false;
    GoalieCentre = false;
    GoalieUncentre = false;
    GoalieStand = false;
    GoalieInitial = false;
    DefenderCentre = false;
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

    COPY(s, GoalieSit)
    COPY(s, GoalieCentre)
    COPY(s, GoalieUncentre)
    COPY(s, GoalieStand)
    COPY(s, GoalieInitial)
    COPY(s, DefenderCentre)
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
    WalkInCircle = WalkInSquare = false;
    Started = false;
    Penalized = false;
    FallenBack = false;
    FallenFront = false;
    Die = false;
    TurnOver = false;
    WalkAside = false;
    WalkForward = false;
    TargetX = 0;
    TargetY = 0;
    TargetTheta = 0;
}

void StrategySpell::CopyTo(Spell *spell)
{
    StrategySpell *s = (StrategySpell*)spell;
    COPY(s, WalkInCircle)
    COPY(s, WalkInSquare)
    COPY(s, Started)
    COPY(s, Penalized)
    COPY(s, FallenBack)
    COPY(s, FallenFront)
    COPY(s, Die)
    COPY(s, TurnOver)
    COPY(s, WalkAside)
    COPY(s, WalkForward)
    COPY(s, TargetX)
    COPY(s, TargetY)
    COPY(s, TargetTheta)
}

void StrategySpell::Load(Storage &storage)
{
    
}

void StrategySpell::Save(Storage &storage)
{
    
}

void StrategySpell::AddOptions(po::options_description &description)
{
    description.add_options()
            ("walk.circle", po::value<bool>()->default_value(false), "walks in circle")
            ("walk.square", po::value<bool>()->default_value(false), "walks in square");
}

void StrategySpell::Update(const po::variables_map &config)
{
    WalkInCircle = config["walk.circle"].as<bool>();
    WalkInSquare = config["walk.square"].as<bool>();
}

BehaviourSpell::BehaviourSpell()
{
    Started = false;
    Penalized = false;
    Fallen = false;
    Die = false;
    LeftEye = WHITE;
    RightEye = WHITE;
}

void BehaviourSpell::CopyTo(Spell *spell)
{
    BehaviourSpell *s = (BehaviourSpell*)spell;
    COPY(s, Started)
    COPY(s, Penalized)
    COPY(s, Fallen)
    COPY(s, Die)
    COPY(s, LeftEye)
    COPY(s, RightEye)
}

void BehaviourSpell::Load(Storage &storage)
{
    
}

void BehaviourSpell::Save(Storage &storage)
{
    
}