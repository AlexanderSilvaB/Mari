#ifndef _MotionModule_H_
#define _MotionModule_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

#ifdef USE_UNSW
#include "rUNSWiftMotionAdapter.h"
#else
#ifdef USE_QIBUILD
#include <RinoLib/Kinetics/Motion.h>
#include <RinoLib/Kinetics/Posture.h>
using namespace Rinobot::Nao;
using namespace Rinobot::Nao::Kinetics;
#endif
#endif

using namespace std;

class MotionModule : public Module
{
    private:
        #ifdef USE_UNSW
        rUNSWiftMotionAdapter *motion;
        #else
        #ifdef USE_QIBUILD
        Motion *motion;
        Posture *posture;
        std::map<string, float> moveConfig;  
        std::vector<string> namesList;
        std::vector<float> anglesList;
        #endif
        #endif

        float headYaw;
        float headPitch;
        float headSpeed;
        bool headRelative;
        float minDistanceToBall;
        float vx, vy, vth;
        bool stiff, _stiff;
        bool stand, _stand;
        bool kickLeft;
        bool kickRight;
        bool limpLeft;
        bool limpRight;
        bool getupFront, getupBack;
        bool walk;

        void ScaleWalk2014(float *forward, float *left, float *turn);
    public:
        MotionModule(SpellBook *spellBook);
        ~MotionModule();
        void Tick(float ellapsedTime);
        void OnStart();
        void OnStop();
};

#endif