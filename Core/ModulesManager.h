#ifndef _MODULES_MANAGER_H_
#define _MODULES_MANAGER_H_

#include "Module.h"
#include <vector>

class ModulesManager
{
    private:
        static vector<Module*> modules;
        static void SignalHandler(int);
    public:
        ModulesManager();
        virtual ~ModulesManager();

        void Attach(Module *module);
        void Dettach(Module *module);

        void Run();
        void Cancel();
        

};

#endif