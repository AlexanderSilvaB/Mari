#include "ModulesManager.h"
#include <algorithm>
#include <signal.h>
#include <setjmp.h>
#include "utils/ConcurrentMap.hpp"

using namespace std;

bool system_running = false;
bool system_cancel = false;
vector<Module*> ModulesManager::modules;

ConcurrentMap<pthread_t, jmp_buf*> jmpPoints;

void SignalHandler(int sigNumber, siginfo_t* info, void*);

ModulesManager::ModulesManager()
{
    system_cancel = false;


    struct sigaction act;
    act.sa_sigaction = SignalHandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO | SA_RESETHAND;

    // register the signal handlers
    if (signal == SIGINT || signal == ALL_SIGNALS)
        sigaction(SIGINT, &act, NULL);   // CTRL-C termination
    //if (signal == SIGTERM || signal == ALL_SIGNALS)
    //    sigaction(SIGTERM, &act, NULL);   // kill -15 termination
    if (signal == SIGSEGV || signal == ALL_SIGNALS)
        sigaction(SIGSEGV, &act, NULL);  // seg fault
    if (signal == SIGFPE || signal == ALL_SIGNALS)
        sigaction(SIGFPE, &act, NULL);   // floating point exception
    if (signal == SIGSTKFLT || signal == ALL_SIGNALS)
        sigaction(SIGSTKFLT, &act, NULL);   // stack faults
    //if (signal == SIGHUP || signal == ALL_SIGNALS)
    //    sigaction(SIGHUP, &act, NULL);   // lost controlling terminal

    //signal (SIGINT, ModulesManager::SignalHandler);    
}

ModulesManager::~ModulesManager()
{
    Cancel();
}

/**
 * The signal handler. Handles the signal and flag that the thread has died
 * and allow the watcher thread to restart it.
 * @param sigNumber The POSIX signal identifier
 * @param info Signal info struct for the signal
 * @see registerSignalHandler
 */
void SignalHandler(int sigNumber, siginfo_t* info, void*)
{
    // End the rUNSWift module [CTRL-C]. Call all destructors
    cout << sigNumber << endl;
    if (sigNumber == SIGINT) 
    {
        cerr << endl;
        cerr << "###########################" << endl;
        cerr << "##    SIGINT RECEIVED    ##" << endl;
        cerr << "##  ATTEMPTING SHUTDOWN  ##" << endl;
        cerr << "###########################" << endl;
        system_cancel = true;
    } 
    else if (sigNumber == SIGTERM) 
    {
        cerr << endl;
        cerr << "###########################" << endl;
        cerr << "##   SIGTERM RECEIVED    ##" << endl;
        cerr << "##  ATTEMPTING SHUTDOWN  ##" << endl;
        cerr << "###########################" << endl;
        system_cancel = true;
    } 
    else 
    {
        // re-register the signal handler
        SAY("crash detected");
        registerSignalHandlers(sigNumber);
        pthread_t thread = pthread_self();

        cerr <<  string(Module::threadName) << " with id " << thread <<
            " received signal " << sigNumber << " and is restarting" << endl;
            llog(ERROR) << string(Module::threadName) << " with id "
                << thread << " received signal "
                << sigNumber << " and is restarting" << endl;

        longjmp(*jmpPoints[thread], 1);
    }
    //system_cancel = true;
}

void ModulesManager::Attach(Module *module)
{
    modules.push_back(module);
    if(system_running)
        module->Start();
}

void ModulesManager::Dettach(Module *module)
{
    vector<Module*>::iterator it = find(modules.begin(), modules.end(), module);
    if(it != modules.end())
    {
        modules.erase(it);
        if(system_running)
            module->Stop();
    }
}

void ModulesManager::Run()
{
    if(system_running)
        return;
    for(int i = 0; i < modules.size(); i++)
    {
        modules[i]->Start();
    }
    system_running = true;
    //modules[modules.size()-1]->Join();
    while(system_running)
    {
        if(system_cancel)
        {
            Cancel();
        }
        usleep(100000);  
    }
}

void ModulesManager::Cancel()
{
    if(!system_running)
        return;
    for(int i = 0; i < modules.size(); i++)
    {
        modules[i]->Stop();
    }
    system_running = false;
}