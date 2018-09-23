#ifndef _SPELLBOOK_H_
#define _SPELLBOOK_H_

#include <string>
#include "Core/Utils/Storage.h"

class Spell
{
    public:
        Spell();
        virtual ~Spell();
        virtual void Load(Storage &storage);
        virtual void Save(Storage &storage);
};

class ModulesSpell : public Spell
{
    public:
        bool LoadStrategy;
        bool LoadRemote;
        bool LoadMotion;
        bool LoadPerception;

        ModulesSpell();
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class BallSpell : public Spell
{
    public:
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
        void Load(Storage &storage);
        void Save(Storage &storage);

};

class PerceptionSpell : public Spell
{
    public:
        bool EnableBallDetector;

        BallSpell ballSpell;

        PerceptionSpell();
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
        bool Walk;

        MotionSpell();
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class RemoteSpell : public Spell
{
    public:
        bool EnableJoystick;
        bool EnableNetwork;

        RemoteSpell();
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class StrategySpell : public Spell
{
    public:
        bool Penalized;

        StrategySpell();
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class SpellBook
{
    public:
        PerceptionSpell perceptionSpell;
        MotionSpell motionSpell;
        ModulesSpell modulesSpell;
        RemoteSpell remoteSpell;
        StrategySpell strategySpell;

        SpellBook();
        void Load(std::string fileName);
        void Save(std::string fileName);
};

#endif