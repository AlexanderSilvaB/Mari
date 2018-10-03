#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <vector>

#include <blackboard/Blackboard.hpp>

#include <gamecontroller/RoboCupGameControlData.hpp>

#include <types/AbsCoord.hpp>
#include <types/ActionCommand.hpp>
#include <types/BallInfo.hpp>
#include <types/BBox.hpp>
#include <types/BehaviourRequest.hpp>
#include <types/BehaviourSharedData.hpp>
#include <types/JointValues.hpp>
#include <types/PostInfo.hpp>
#include <types/RobotInfo.hpp>
#include <types/RobotObstacle.hpp>
#include <types/RRCoord.hpp>
#include <types/XYZ_Coord.hpp>
#include <types/SensorValues.hpp>
#include <types/TeamBallInfo.hpp>

#include <utils/body.hpp>
#include <utils/speech.hpp>
#include <utils/SPLDefs.hpp>

#include "RegisterConverters.hpp"

using namespace boost::python;

typedef const float (RRCoord ::* rr_get_v_const_type)() const;
typedef const float (AbsCoord::*abs_get_v_const_type)() const;

void py_say(const std::string &text) { SAY(text); }

BOOST_PYTHON_MODULE(robot)
{
   register_python_converters();

   #include "wrappers/AbsCoord_wrap.pcpp"
   #include "wrappers/ActionCommand_wrap.pcpp"
   #include "wrappers/BallInfo_wrap.pcpp"
   #include "wrappers/BBox_wrap.pcpp"
   #include "wrappers/BehaviourBlackboard_wrap.pcpp"
   #include "wrappers/BehaviourRequest_wrap.pcpp"
   #include "wrappers/BehaviourSharedData_wrap.pcpp"
   #include "wrappers/Blackboard_wrap.pcpp"
   #include "wrappers/body_wrap.pcpp"
   #include "wrappers/BroadcastData_wrap.pcpp"
   #include "wrappers/GameController_wrap.pcpp"
   #include "wrappers/GameControllerBlackboard_wrap.pcpp"
   #include "wrappers/KinematicsBlackboard_wrap.pcpp"
   #include "wrappers/JointValues_wrap.pcpp"
   #include "wrappers/LocalisationBlackboard_wrap.pcpp"
   #include "wrappers/MotionBlackboard_wrap.pcpp"
   #include "wrappers/PostInfo_wrap.pcpp"
   #include "wrappers/ReceiverBlackboard_wrap.pcpp"
   #include "wrappers/RobotInfo_wrap.pcpp"
   #include "wrappers/RobotObstacle_wrap.pcpp"
   #include "wrappers/RRCoord_wrap.pcpp"
   #include "wrappers/XYZ_Coord_wrap.pcpp"
   #include "wrappers/say_wrap.pcpp"
   #include "wrappers/SensorValues_wrap.pcpp"
   #include "wrappers/SPLDefs_wrap.pcpp"
   #include "wrappers/VisionDefs_wrap.pcpp"
   #include "wrappers/vector_wrap.pcpp"
   #include "wrappers/VisionBlackboard_wrap.pcpp"
   #include "wrappers/TeamBallInfo_wrap.pcpp"
   #include "wrappers/ReadySkillPositionAllocation_wrap.pcpp"

}
