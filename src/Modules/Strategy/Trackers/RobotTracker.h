#ifndef ROBOT_TRACKER_H_
#define ROBOT_TRACKER_H_

#include "Eigen/Eigen"
#include "Core/Module.h"
#include "Core/SpellBook.h"

class RobotTracker : public InnerModule
{
    private:
        Eigen::Vector3f X, U;
    public:
        RobotTracker(SpellBook *spellBook);
        void OnStart();
        void Tick(float ellapsedTime);
};

#endif
