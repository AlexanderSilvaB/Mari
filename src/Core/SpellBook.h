#ifndef _SPELLBOOK_H_
#define _SPELLBOOK_H_

#include <string>
#include "Core/Utils/Storage.h"
#include "Core/Utils/Colors.h"
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include "Core/InitManager.h"
#include <pthread.h>
#include "GC/RoboCupGameControlData.h"

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
        int ImageX, ImageY;
        int BallLostCount;

        bool BallDetected;
        float BallYaw;
        float BallPitch;
        float BallDistance;
        float HeadYaw;
        float HeadPitch;
        float HeadSpeed;
        bool HeadRelative;
        float TimeSinceBallSeen;

        BallSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class FeatureSpell : public Spell
{
    public:
        bool Enabled;
        enum Type { L, T, X, Unknown };
        int ImageX, ImageY;
        int FeatureLostCount;

        bool FeatureDetected;
        float FeatureYaw;
        float FeaturePitch;
        float FeatureDistance;
        float HeadYaw;
        float HeadPitch;
        float HeadSpeed;
        bool HeadRelative;
        float TimeSinceFeatureSeen;

        FeatureSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
};


class RobotDetectorSpell : public Spell
{
    public:
        bool Enabled;

        RobotDetectorSpell();
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

class VisionSpell : public Spell
{
    public:
        bool Enabled;
        bool Record;
        std::string VideoName;
        bool BGR, HSV, GRAY;

        BallSpell ball;
        LocalizationSpell localization;
        FeatureSpell feature;
        RobotDetectorSpell robotDetector;

        VisionSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);

};


class PerceptionSpell : public Spell
{
    public:
        VisionSpell vision;

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
        float HeadSpeedYaw, HeadSpeedPitch;
        bool HeadRelative;
        bool KickLeft, KickRight;
        bool LimpLeft, LimpRight;
        bool GetupFront, GetupBack;
        bool TipOver, Dead;
        bool Walk;
        bool Crouch;

        bool GoalieSit;
        bool GoalieCentre;
        bool GoalieUncentre;
        bool GoalieStand;
        bool GoalieInitial;
        bool DefenderCentre;
        bool ThrowIn;

        bool Calibrate;
        float AngleX, AngleY, GyroX, GyroY;

        MotionSpell();
        void CopyTo(Spell *spell);
        void AddOptions(boost::program_options::options_description &description);
        void Update(const boost::program_options::variables_map& config);
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

class GameControllerSpell : public Spell
{
    public:
        bool Connected;
        uint8_t GameState;
        bool PenaltyPhase;

        GameControllerSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class NetworkSpell : public Spell
{
    public:
        struct CameraSettings 
        {
            bool brightnessChanged;
            unsigned int brightness;
            bool saturationChanged;
            unsigned int saturation;
            bool contrastChanged;
            unsigned int contrast;
            bool sharpnessChanged;
            unsigned int sharpness;
        };

        bool TCPConnected;
        int SelectedCamera;
        CameraSettings topSettings, botSettings;
        GameControllerSpell gameController;

        NetworkSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);
};

class StrategySpell : public Spell
{
    public:
        uint8_t GameState;
        bool WalkInCircle, WalkInSquare;
        bool Started;
        bool Penalized;
        bool FallenFront, FallenBack;
        bool Die, TurnOver;

        bool WalkForward;
        bool WalkAside;
        float TargetX, TargetY, TargetTheta;

        float HeadYawRange, HeadPitchRange;
        float HeadSearchSpeed;
        int HeadScanCount;
        bool MoveHead;
        bool FakeKick;
        bool Defensive;

        float TimeSinceStarted;
        float TimeSincePenalized;


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
        std::string Name;
        int Number;
        int TeamNumber;

        BehaviourSpell();
        void CopyTo(Spell *spell);
        void Load(Storage &storage);
        void Save(Storage &storage);

};

#define COPY(s, var) s->var = var;
#define COPYMEM(s, var) memcpy(&(s->var), &var, sizeof(var));
#define LOAD(module)    spellBookBase->Lock(); \
                        spellBookBase->module.CopyTo(&(spellBook->module)); \
                        spellBookBase->Unlock();
#define SAVE(module)    spellBookBase->Lock(); \
                        spellBook->module.CopyTo(&(spellBookBase->module)); \
                        spellBookBase->Unlock();

//#define LOAD(module) int inutil_##module;
//#define SAVE(module) int inutil_##module;

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
        NetworkSpell network;

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