#include "Module.h"
#include "Utils/SimpleTimer.h"
#include "Utils/Math.h"
#include <iostream>

using namespace std;

Module::Module(SpellBook *spellBook, std::string name, int ms)
{   
    is_running = false;
    highPriority = false;
    this->name = name;
    this->ms = ms;
    this->spellBook = spellBook;
}

Module::~Module()
{
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
    int rc;
    pthread_join(thread, NULL);
    OnStop();
}

void *Module::Run(void *arg)
{
    Module *module = (Module *)arg;
    SimpleTimer timer, timerWait;
    float t;
    int ms = 0, wt = 0;
    while (module->is_running)
    {
        t = timer.Seconds();
        if(t < 0.0f)
            t = -t;
        timerWait.Restart();
        module->Tick(t);
        timer.Restart();
        ms = timerWait.Millis();
        
        wt = max(module->ms - ms, 10);
        usleep(wt*1000);
    }
    pthread_exit(NULL);
}