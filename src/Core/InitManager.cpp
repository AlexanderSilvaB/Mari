#include <boost/program_options.hpp>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sys/file.h>

#include "soccer.hpp"

#include "utils/Logger.hpp"
#include "utils/options.hpp"

#include "thread/ThreadManager.hpp"
#include "motion/MotionAdapter.hpp"
#include "perception/PerceptionThread.hpp"
#include "perception/vision/NaoCamera.hpp"
#include "perception/vision/NaoCameraV4.hpp"
#include "Utils/CombinedCamera.hpp"
#include "Utils/set_cloexec.hpp"

#include "SpellBook.h"

#include "InitManager.h"

namespace po = boost::program_options;
using namespace std;
using namespace boost;

int InitManager::pid_file = 0;
po::variables_map InitManager::vm;
Blackboard *InitManager::blackboard = NULL;
Camera *InitManager::topCamera = NULL;
Camera *InitManager::botCamera = NULL;

void InitManager::Init(int argc, char *argv[], SpellBook &spellBook)
{
    int pid_file = open("/var/volatile/rinobot.pid", O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if (rc)
    {
        if (EWOULDBLOCK == errno)
        {
            // another instance is running
            std::cerr << "\033[31mAnother rinobot is already running!\033[0m" << std::endl;
            std::cerr << "Things to try to get your rinobot running: \n"
                      << "   \033[36m killall rinobot \033[0m \n"
                      << "   \033[36m nao restart \033[0m" << std::endl;
            std::cerr << "If that fails, then try: \n"
                      << "   \033[36m lsof -n | grep rinobot \033[0m \n"
                      << "The second number is rinobot's <pid>, and then: \n"
                      << "   \033[36m kill -9 <pid> \033[0m" << std::endl;
            exit(1);
        }
    }
    else
    {
        // this is the first instance
        set_cloexec_flag(pid_file);
    }
    ParseArgs(argc, argv, spellBook);

    InitManager::InitCameras();
}

void InitManager::ParseArgs(int argc, char *argv[], SpellBook &spellBook)
{
    try
    {
        po::options_description generic("Generic options");
        generic.add_options()("help,h", "produce help message")("version,v", "print version string");

        po::options_description rinobot("Rinobot options");
        spellBook.AddOptions(rinobot);


        generic.add(rinobot);
        po::options_description cmdline_options = store_and_notify(argc, argv, vm, &rinobot);

        if (vm.count("help"))
        {
            cout << cmdline_options << endl;
            return 1;
        }

        if (vm.count("version"))
        {
            cout << "Rinobot Nao soccer player " << VERSION_MAJOR << "." << VERSION_MINOR << endl;
            return 1;
        }

        cout << "Rinobot V." << VERSION_MAJOR << "." << VERSION_MINOR << endl;

        options_print(vm);
    }
    catch (program_options::error &e)
    {
        cerr << "Error when parsing command line arguments: " << e.what() << endl;
        //return 1;
    }
    catch (std::exception &e)
    {
        cerr << e.what() << endl;
        return 1;
    }

    offNao = false;
    attemptingShutdown = false;
    Thread::name = "main";

    time_t s_since_epoch = time(NULL);
    const struct tm *now = localtime(&s_since_epoch);
    char timestr[20];
    strftime(timestr, 20, "%Y-%m-%d-%H-%M-%S", now);

    Logger::init(vm["debug.logpath"].as<string>() + string("/") + string(timestr),
                 vm["debug.log"].as<string>(),
                 vm["debug.log.motion"].as<bool>());

    blackboard = new Blackboard(vm);
}

void InitManager::InitCameras()
{
    cout << "InitCameras" << endl;
    llog(INFO) << "Initialising v4 /dev/video0" << std::endl;
    topCamera = new NaoCameraV4(blackboard, "/dev/video0",
                                IO_METHOD_MMAP,
                                AL::kQVGA, TOP_CAMERA);

    llog(INFO) << "Initialising v4 /dev/video1" << std::endl;
    botCamera = new NaoCameraV4(blackboard, "/dev/video1",
                                IO_METHOD_MMAP,
                                AL::kQVGA, BOTTOM_CAMERA);

    CombinedCamera::setCameraTop(topCamera);
    CombinedCamera::setCameraBot(botCamera);
}

Blackboard *InitManager::GetBlackboard()
{
    return blackboard;
}

void InitManager::Destroy()
{
    delete blackboard;

    delete topCamera;
    delete botCamera;
}