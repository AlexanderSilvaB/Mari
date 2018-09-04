#ifndef RINOBOT_MODULE_H
#define RINOBOT_MODULE_H
#include <pthread.h>
#include <unistd.h>
#include "SpellBook.h"

using namespace std;

class Module
{
  public:
    Module(SpellBook*, int);
    bool IsRunning();
    bool IsHighPriority();
    virtual ~Module();
    virtual void Tick(float ellapsedTime);
    void Start();
    void Stop();
    virtual void OnStop();
    virtual void OnStart();

  protected:
    SpellBook *spellBook;
    bool highPriority;

  private:
    static void *Run(void *arg);
    pthread_t thread;
    int ms;
    bool is_running;
};

#endif