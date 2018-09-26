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
    Module(SpellBook*, std::string, int);
    void SetHighPriority(bool highPriority);
    bool IsRunning();
    bool IsHighPriority();
    std::string Name();
    virtual ~Module();
    virtual void Tick(float ellapsedTime);
    void Start();
    void Stop();
    void Join();
    virtual void OnStop();
    virtual void OnStart();

  protected:
    SpellBook *spellBook;

  private:
    static void *Run(void *arg);
    pthread_t thread;
    int us;
    bool is_running;
    bool highPriority;
    std::string name;
};

#endif