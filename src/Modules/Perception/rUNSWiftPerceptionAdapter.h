#ifndef _RUNSWIFTPERCEPTIONADAPTER_H_
#define _RUNSWIFTPERCEPTIONADAPTER_H_

#include "Core/rUNSWiftAdapter.h"
#include <string>
#include "perception/vision/VisionAdapter.hpp"
#include "perception/localisation/LocalisationAdapter.hpp"
#include "perception/kinematics/KinematicsAdapter.hpp"
#include "perception/dumper/PerceptionDumper.hpp"
#include "blackboard/Adapter.hpp"

#define THREAD_MAX_TIME 33666

#define TICK_MAX_TIME_VISION 30000
#define TICK_MAX_TIME_KINEMATICS 30000
#define TICK_MAX_TIME_LOCALISATION 30000
#define TICK_MAX_TIME_BEHAVIOUR 30000

class rUNSWiftPerceptionAdapter : public rUNSWiftAdapter
{
    public:
        rUNSWiftPerceptionAdapter();
        ~rUNSWiftPerceptionAdapter();
        void Start();
        void Stop();
        void Tick();
        void ReadOptions(const boost::program_options::variables_map& config);

    private:
        KinematicsAdapter *kinematicsAdapter;
        VisionAdapter *visionAdapter;
        LocalisationAdapter *localisationAdapter;

        PerceptionDumper *dumper;
        Timer dump_timer;
        unsigned int dump_rate;
};

#endif