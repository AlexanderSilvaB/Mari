#ifndef _MotionModule_H_
#define _MotionModule_H_

#include "Core/Module.h"
#include <vector>
#include <string>
#include <map>

#include "rUNSWiftMotionAdapter.h"

using namespace std;

class MotionModule : public Module
{
    private:
        bool saveData;
        rUNSWiftMotionAdapter *motion;

        float vx, vy, vth;
        bool stiff, stand, goalieStand;

        void ScaleWalk2014(float *forward, float *left, float *turn);
    public:
        MotionModule(SpellBook *spellBook);
        ~MotionModule();
        void Tick(float ellapsedTime);
        void OnStart();
        void OnStop();
        void Load();
        void Save();
};

#endif