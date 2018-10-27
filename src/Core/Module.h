#ifndef RINOBOT_MODULE_H
#define RINOBOT_MODULE_H
#include <stdint.h>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include "SpellBook.h"

using namespace std;

class Module
{
  public:
    static __thread char* threadName;

    
    Module(SpellBook*, std::string, int);
    void SetPriority(int priority);
    void SetMaxTime(int maxTime);
    bool IsRunning();
    int GetPriority();
    int GetMaxTime();
    std::string Name();
    virtual ~Module();
    virtual void Tick(float ellapsedTime);
    void Start();
    void Stop();
    virtual void Load();
    virtual void Save();
    void Join();
    virtual void OnStop();
    virtual void OnStart();

  protected:
    SpellBook *spellBook;
    SpellBook *spellBookBase;

  private:
    static void *Run(void *arg);
    pthread_t thread;
    int us;
    bool is_running;
    int priority;
    int maxTime;
    std::string name;

    static void overtimeAlert(int);
};

class InnerModule
{
  public:
    InnerModule(SpellBook*);
    virtual ~InnerModule();
    virtual void Tick(float ellapsedTime);
    virtual void OnStop();
    virtual void OnStart();
  protected:
    SpellBook *spellBook;
};

#endif