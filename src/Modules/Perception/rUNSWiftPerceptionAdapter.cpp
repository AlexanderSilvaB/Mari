#include "rUNSWiftPerceptionAdapter.h"

#include <pthread.h>
#include <ctime>
#include <utility>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "perception/dumper/PerceptionDumper.hpp"
//#include "perception/behaviour/SafetySkill.hpp"
#include "soccer.hpp"
#include "blackboard/Blackboard.hpp"
#include "utils/Logger.hpp"
#include "thread/Thread.hpp"
#include "boost/lexical_cast.hpp"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "Core/InitManager.h"

using namespace std;
using namespace boost;

rUNSWiftPerceptionAdapter::rUNSWiftPerceptionAdapter()
    : rUNSWiftAdapter()
{
    kinematicsAdapter = new KinematicsAdapter(blackboard);
    localisationAdapter = new LocalisationAdapter(blackboard);
    visionAdapter = new VisionAdapter(blackboard);
}

rUNSWiftPerceptionAdapter::~rUNSWiftPerceptionAdapter()
{
    
}

void rUNSWiftPerceptionAdapter::Start()
{
    dumper = NULL;

    releaseLock(serialization);


    ReadOptions(blackboard->config);
    writeTo(thread, configCallbacks["perception"],
           boost::function<void(const boost::program_options::variables_map &)>
           (boost::bind(&rUNSWiftPerceptionAdapter::ReadOptions, this, _1)));
}

void rUNSWiftPerceptionAdapter::Stop()
{
    llog(INFO) << __PRETTY_FUNCTION__ << endl;
    writeTo(thread, configCallbacks["perception"], boost::function<void(const boost::program_options::variables_map &)>());
}

void rUNSWiftPerceptionAdapter::ReadOptions(const boost::program_options::variables_map &config)
{
    const string &e = config["vision.camera_controls"].as<string>();
   vector<string> vs;
   split(vs, e, is_any_of(",;"));
   for (vector<string>::const_iterator ci = vs.begin(); ci != vs.end(); ++ci) {
      vector<string> nv;
      split(nv, *ci, is_any_of(":"));
      if (nv.size() != 3)
         llog(ERROR) << "controls should be cam:control_id:value" << endl;
      else{
         Camera *currCamera;
         if(strtol(nv[0].c_str(),NULL,10) == 0){
            currCamera = visionAdapter->combined_camera_->getCameraBot();
         }else{
            currCamera = visionAdapter->combined_camera_->getCameraTop();
         }
         if(strtoul(nv[1].c_str(), NULL, 10) == 0)
            currCamera->setControl(22,1);
         if(strtoul(nv[1].c_str(), NULL, 10) == 17)
            currCamera->setControl(22,0);
         if(strtoul(nv[1].c_str(), NULL, 10) == 19)
            currCamera->setControl(22,0);
         currCamera->setControl(strtoul(nv[1].c_str(), NULL, 10),
                                strtol (nv[2].c_str(), NULL, 10));
      }
   }
}

void rUNSWiftPerceptionAdapter::Tick()
{
   llog_open(VERBOSE) << "Perception Thread" << endl;

   Timer timer_thread;
   Timer timer_tick;

   /*
    * Kinematics Tick
    */
   llog_open(VERBOSE) << "Kinematics Tick" << endl;
   timer_tick.restart();
   kinematicsAdapter->tick();

   uint32_t kinematics_time = timer_tick.elapsed_us();
   if (kinematics_time > TICK_MAX_TIME_KINEMATICS) {
      llog_close(VERBOSE) << "Kinematics Tick: OK " << kinematics_time << " us" << endl;
   } else {
      llog_close(ERROR) << "Kinematics Tick: TOO LONG " << kinematics_time << " us" << endl;
   }

    /*
    * Vision Tick
    */
   llog_open(VERBOSE) << "Vision Tick" << endl;
   timer_tick.restart();
   visionAdapter->tick();

   uint32_t vision_time = timer_tick.elapsed_us();
   if (vision_time > TICK_MAX_TIME_VISION) {
      llog_close(VERBOSE) << "Vision Tick: OK " << vision_time << endl;
   } else {
      llog_close(ERROR) << "Vision Tick: TOO LONG " << vision_time << endl;
   }

   /*
    * Localisation Tick
    */
   llog_open(VERBOSE) << "Localisation Tick" << endl;
   timer_tick.restart();
   localisationAdapter->tick();

   uint32_t localisation_time = timer_tick.elapsed_us();
   if (localisation_time > TICK_MAX_TIME_LOCALISATION) {
      llog_close(VERBOSE) << "Localisation Tick: OK " << localisation_time << endl;
   } else {
      llog_close(ERROR) << "Localisation Tick: TOO LONG " << localisation_time << endl;
   }

   /*
    * Finishing Perception
    */
   uint32_t perception_time = timer_thread.elapsed_us();
   if (perception_time > THREAD_MAX_TIME) {
      llog_close(VERBOSE) << "Perception Thread: OK " << perception_time << endl;
   } else {
      llog_close(ERROR) << "Perception Thread: TOO LONG " << perception_time << endl;
   }

   writeTo(perception, kinematics, kinematics_time);
   writeTo(perception, vision, vision_time);
   writeTo(perception, localisation, localisation_time);
   writeTo(perception, total, perception_time);

   if (dumper) {
      if (dump_timer.elapsed_us() > dump_rate) {
         dump_timer.restart();
         try {
            dumper->dump(blackboard);
         } catch(const std::exception &e) {
            attemptingShutdown = true;
            cout << "Error: " << e.what() << endl;
         }
      }
   }
}
