#include "RobotTracker.h"

#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

using namespace std;
using namespace Eigen;

RobotTracker::RobotTracker(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
       
}

void RobotTracker::OnStart()
{
    spellBook->perception.vision.localization.X = 0;
    spellBook->perception.vision.localization.Y = 0;
    spellBook->perception.vision.localization.Theta = 0;
}

void RobotTracker::Tick(float ellapsedTime)
{
    X(0) = spellBook->perception.vision.localization.X;
    X(1) = spellBook->perception.vision.localization.Y;
    X(2) = spellBook->perception.vision.localization.Theta;

    U(0) = spellBook->motion.Vx;
    U(1) = spellBook->motion.Vy;
    U(2) = spellBook->motion.Vth;


    float forward = U(0) * cos(-U(2)) - U(1) * sin(-U(2));
    float left =    U(0) * sin(-U(2)) + U(1) * cos(-U(2));

    X(0) = X(0) + forward*ellapsedTime;
    X(1) = X(1) + left*ellapsedTime;
    X(2) = X(2) + U(2)*ellapsedTime;

    X(2) = FixAngle(X(2));

    nlog(LEVEL_INFO) << "Odometry: " << X << endl;

    spellBook->perception.vision.localization.X = X(0);
    spellBook->perception.vision.localization.Y = X(1);
    spellBook->perception.vision.localization.Theta = X(2);
}
