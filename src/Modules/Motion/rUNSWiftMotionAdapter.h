#ifndef _RUNSWIFTMOTIONADAPTER_H_
#define _RUNSWIFTMOTIONADAPTER_H_

#include "Core/rUNSWiftAdapter.h"
#include <string>
#include <map>
#include "motion/effector/Effector.hpp"
#include "motion/generator/Generator.hpp"
#include "motion/touch/FilteredTouch.hpp"
#include "blackboard/Adapter.hpp"
#include "motion/generator/BodyModel.hpp"
#include "perception/kinematics/Kinematics.hpp"
#include "motion/SonarRecorder.hpp"

class rUNSWiftMotionAdapter : public rUNSWiftAdapter
{
    public:
        rUNSWiftMotionAdapter();
        ~rUNSWiftMotionAdapter();
        void Start();
        void Stop();
        void Tick(ActionCommand::All &request);
        SensorValues GetSensors();
        SensorValues GetRawSensors();
        void ReadOptions(const boost::program_options::variables_map& config);

    private:
        std::vector<Odometry> odometryBuffer;
        /* Buffers so synchronises with vision thread */
        std::vector<SensorValues> sensorBuffer;
        /* Sonar window recorder */
        SonarRecorder sonarRecorder;
        /* Duration since we last were told to stand up by libagent (seconds) */
        float uptime;
        /* Maps of available switchable instances */
        std::map<std::string, Touch*> touches;
        std::map<std::string, Effector*> effectors;
        /* Motion module instance */
        Touch* nakedTouch;  //original, unfiltered
        Touch* touch;
        Generator* generator;
        Effector* effector;
        BodyModel bodyModel;
        Kinematics kinematics;
};

#endif