#ifndef _SPELLBOOK_H_
#define _SPELLBOOK_H_

#include <string>
#include "Core/Utils/Storage.h"
#include "Core/Utils/Colors.h"
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include "Core/InitManager.h"
#include <pthread.h>

class Spell
{
    public:
        Spell();
        virtual ~Spell();
        virtual void CopyTo(Spell *spell);
        virtual void Load(Storage &storage);
        virtual void Save(Storage &storage);
        virtual void AddOptions(boost::program_options::options_description &description);
        virtual void Update(const boost::program_options::variables_map& config);
};

class ModulesSpell : public Spell
{
    public:
        bool LoadStrategy;
        bool LoadRemote;
        bool LoadMotion;
        bool LoadPerception;
        bool LoadBehaviour;
        bool LoadNetwork;

        ModulesSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class BallSpell : public Spell
{
    public:
        bool Enabled;
        std::string method;
        float ballWidth, ballHeight;

        bool BallDetected;
        float BallAzimuth;
        float BallElevation;
        float BallDistance;
        float HeadSpeed;
        bool HeadRelative;
        float TimeSinceBallSeen;

        BallSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);

};

class LocalizationSpell : public Spell
{
    public:
        bool Enabled;
        float X, Y, Theta;

        LocalizationSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);

};

class PerceptionSpell : public Spell
{
    public:
        BallSpell ball;
        LocalizationSpell localization;

        PerceptionSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class MotionSpell : public Spell
{
    public:
        bool Remote;
        bool Stand, Stiff;
        float Vx, Vy, Vth;
        float HeadYaw, HeadPitch;
        float HeadSpeed;
        bool HeadRelative;
        bool KickLeft, KickRight;
        bool LimpLeft, LimpRight;
        bool GetupFront, GetupBack;
        bool TipOver, Dead;
        bool Walk;
        bool Crouch;

        MotionSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class RemoteSpell : public Spell
{
    public:
        bool EnableJoystick;
        bool EnableNetwork;

        RemoteSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class StrategySpell : public Spell
{
    public:
        bool WalkInCircle, WalkInSquare;
        bool Started;
        bool Penalized;
        bool FallenFront, FallenBack;
        bool Die, TurnOver;

        StrategySpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
        void AddOptions(boost::program_options::options_description &description);
        void Update(const boost::program_options::variables_map& config);
};

class BehaviourSpell : public Spell
{
    public:
        bool Started;
        bool Penalized;
        bool Fallen;
        bool Die;
        int LeftEye;
        int RightEye;

        BehaviourSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);

};

#define COPY(s, var) s->var = var;
#define LOAD(module)    spellBookBase->Lock(); \
                        spellBookBase->module.CopyTo(&(spellBook->module)); \
                        spellBookBase->Unlock();
#define SAVE(module)    spellBookBase->Lock(); \
                        spellBook->module.CopyTo(&(spellBookBase->module)); \
                        spellBookBase->Unlock();

class SpellBook
{
    private:
        pthread_mutex_t lock;
    public:
        PerceptionSpell perception;
        MotionSpell motion;
        ModulesSpell modules;
        RemoteSpell remote;
        StrategySpell strategy;
        BehaviourSpell behaviour;

        SpellBook();
        ~SpellBook();
        void Load(std::string fileName);
        void Save(std::string fileName);
        void AddOptions(boost::program_options::options_description &description);
        void Update();

        void Lock();
        void Unlock();
};

#endif