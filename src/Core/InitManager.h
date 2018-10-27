#ifndef _INITMANAGER_H_
#define _INITMANAGER_H_

#include <boost/program_options.hpp>
#include "thread/ThreadManager.hpp"
#include "perception/vision/camera/NaoCamera.hpp"
#include "utils/speech.hpp"

class SpellBook;

class InitManager
{
    private:
        static int pid_file;
        static boost::program_options::variables_map vm;
        static Blackboard *blackboard;
        static Camera *topCamera, *botCamera;

        static void ParseArgs(int argc, char *argv[], SpellBook &spellBook);
    public:
        static void Init(int argc, char *argv[], SpellBook &spellBook);
        static void InitCameras();
        static Blackboard *GetBlackboard();
        static void Destroy();
};

#endif