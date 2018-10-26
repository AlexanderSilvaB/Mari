#include "Module.h"
#include "utils/Timer.hpp"
#include "Utils/Math.h"
#include <iostream>

using namespace std;

extern ConcurrentMap<pthread_t, jmp_buf*>     jmpPoints;
__thread char* Module::threadName = NULL;

#define MAX(v1, v2) (v1 > v2 ? v1 : v2)
#define MIN(v1, v2) (v1 < v2 ? v1 : v2)

Module::Module(SpellBook *spellBook, std::string name, int ms)
{   
    is_running = false;
    highPriority = false;
    maxTime = 0;
    this->name = name;
    this->us = ms*1000;
    this->spellBookBase = spellBook;
    this->spellBook = new SpellBook();
}

Module::~Module()
{
    delete spellBook;
}

void Module::SetMaxTime(int maxTime)
{
    this->maxTime = maxTime;
}

int Module::GetMaxTime()
{
    return maxTime;
}

void Module::SetHighPriority(bool highPriority)
{
    this->highPriority = highPriority;
}

bool Module::IsRunning()
{
    return is_running;
}

bool Module::IsHighPriority()
{
    return highPriority;
}

string Module::Name()
{
    return name;
}

void Module::Tick(float ellapsedTime)
{
    
}

void Module::OnStop()
{

}

void Module::OnStart()
{

}

void Module::Load()
{

}

void Module::Save()
{

}

void Module::Start()
{
    cout << "Starting " << name << endl;
    is_running = true;
    OnStart();
    int rc;
    if(!highPriority)
    {
        rc = pthread_create(&thread, NULL, Module::Run, (void *)this); //rc = returned code
    }
    else
    {
        struct sched_param param;
        int policy;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        param.sched_priority = 65;
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&thread, &attr, Module::Run, (void *)this);
        pthread_attr_destroy(&attr);
        pthread_getschedparam(thread, &policy, &param);
    }
    cout << name << " started" << endl;
}

void Module::Stop()
{
    cout << "Stopping " << name << endl;
    is_running = false;
    Join();
    cout << name << " stopped" << endl;
}

void Module::Join()
{
    pthread_join(thread, NULL);
    OnStop();
}

void *Module::Run(void *arg)
{
    Module *module = (Module *)arg;

    Module::threadName = module->name.c_str();
    pthread_t threadID = pthread_self();

    Timer timer, timerWait;
    float t;
    int us = 0, wt = 0;

    if(module->maxTime > 0)
    {
        signal(SIGALRM, overtimeAlert);
    }

    while (module->is_running)
    {
        try
        {
            // we don't care if this leaks
            // but it shouldn't reach this line again in this thread
            jmpPoints[threadID] = reinterpret_cast<jmp_buf*>(malloc(sizeof(jmp_buf)));
            if (!jmpPoints[threadID])
                llog(FATAL) << "malloc failed for" << module->name << "\n";

            llog(INFO) << "Thread '" << module->name << "' started\n";
            if (!setjmp(*jmpPoints[threadID])) 
            {
                while (module->is_running)
                {
                    t = timer.elapsed_us() * 0.000001f;
                    timer.restart();
                    //t = timer.elapsed();
                    if(t < 0.0f)
                        t = -t;

                    if (module->maxTime > 0) 
                    {
                        struct itimerval itval5;
                        itval5.it_value.tv_sec = module->maxTime / 1000;
                        itval5.it_value.tv_usec = 0;
                        itval5.it_interval.tv_sec = 0;
                        itval5.it_interval.tv_usec = 0;
                        setitimer(ITIMER_REAL, &itval5, NULL);
                    }

                    timerWait.restart();
                    module->Load();
                    module->Tick(t);
                    module->Save();

                    if (module->maxTime > 0) 
                    {
                        struct itimerval itval0;
                        itval0.it_value.tv_sec = 0;
                        itval0.it_value.tv_usec = 0;
                        itval0.it_interval.tv_sec = 0;
                        itval0.it_interval.tv_usec = 0;
                        setitimer(ITIMER_REAL, &itval0, NULL);
                    }

                    us = timerWait.elapsed_us();
                    
                    wt = min(max(module->us - us, 50), 5000000);
                    //cout << module->Name() << " -> " << module->us << " - " << us << " = " << wt << endl;
                    usleep(wt);
                }
            }
            llog(INFO) << "Thread '" << module->name << "' disabled." << std::endl;
        }
        catch(const std::exception & e) 
        {
            SAY("exception caught");
            free(jmpPoints[threadID]);
            usleep(500000);
            llog(ERROR) << "exception derivative was caught with error msg: " << e.what() << std::endl;
            llog(ERROR) << "in " << module->name << " with id " << threadID << std::endl;
        } catch(...) 
        {
            SAY("exception caught");
            free(jmpPoints[threadID]);
            usleep(500000);
            llog(ERROR) << "Something was thrown from " << module->name << " with id " << threadID << std::endl;
        }
    }
    pthread_exit(NULL);
}

InnerModule::InnerModule(SpellBook *spellBook)
{
    this->spellBook = spellBook;
}

InnerModule::~InnerModule()
{
    
}

void InnerModule::Tick(float ellapsedTime)
{
    
}

void InnerModule::OnStop()
{

}

void InnerModule::OnStart()
{

}


void Module::overtimeAlert(int) 
{
    cout << "overtime" << endl;
    //SAY(std::string(Module::threadName) + " thread has frozen");
    SAY("thread has frozen");
    signal(SIGALRM, overtimeAlert);
    struct itimerval itval5;
    itval5.it_value.tv_sec = 5;
    itval5.it_value.tv_usec = 0;
    itval5.it_interval.tv_sec = 0;
    itval5.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &itval5, NULL);
}