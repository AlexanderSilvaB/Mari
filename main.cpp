#include <iostream>
#include <unistd.h>
#include "Core/InitManager.h"
#include "unsw/perception/vision/camera/CombinedCamera.hpp"
#include "Core/ModulesManager.h"
#include "Modules/Ball/BallDetector.h"
#include "Modules/Motion/MotionModule.h"
#include "Modules/Remote/RemoteModule.h"
#include "Modules/Perception/PerceptionModule.h"

using namespace std;

int main(int argc,char *argv[])
{
    InitManager::Init(argc, argv);

    ModulesManager manager;
    SpellBook spellBook;
    spellBook.Load("Config.json");

    if(spellBook.modulesSpell.LoadVision)
        manager.Attach(new BallDetector(&spellBook));
    if(spellBook.modulesSpell.LoadPerception)
        manager.Attach(new PerceptionModule(&spellBook));
    if(spellBook.modulesSpell.LoadMotion)
    {
        manager.Attach(new MotionModule(&spellBook));
        manager.Attach(new RemoteModule(&spellBook));
    }
    
    manager.Run();

    InitManager::Destroy();

    return 0;
}