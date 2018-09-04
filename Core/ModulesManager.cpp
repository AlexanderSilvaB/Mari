#include "ModulesManager.h"
#include <algorithm>
#include <signal.h>

using namespace std;

bool system_running = false;

ModulesManager::ModulesManager()
{
    
}

ModulesManager::~ModulesManager()
{
    Cancel();
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
    while(system_running)
    {
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