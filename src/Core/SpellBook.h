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
        bool LoadVision;
        bool LoadMotion;
        bool LoadPerception;

        ModulesSpell();
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class VisionSpell : public Spell
{
    public:
        bool BallDetected;
        float BallAzimuth;
        float BallElevation;
        float BallDistance;
        float HeadSpeed;
        bool HeadRelative;
        float TimeSinceBallSeen;

        VisionSpell();
        void Load(Storage &storage);
        void Save(Storage &storage);

};

class MotionSpell : public Spell
{
    public:
        float HeadAzimuth;

        bool RemoteMotion;
        bool RemoteStand, RemoteStiff;
        float RemoteVx, RemoteVy, RemoteVth;
        float RemoteHeadYaw, RemoteHeadPitch;
        bool RemoteKickLeft, RemoteKickRight;
        bool RemoteLimpLeft, RemoteLimpRight;

        MotionSpell();
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class SpellBook
{
    public:
        VisionSpell visionSpell;
        MotionSpell motionSpell;
        ModulesSpell modulesSpell;

        SpellBook();
        void Load(std::string fileName);
        void Save(std::string fileName);
};

#endif