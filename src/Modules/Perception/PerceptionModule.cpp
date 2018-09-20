#include "PerceptionModule.h"
#include "rUNSWiftPerceptionAdapter.h"
#include "Core/InitManager.h"

PerceptionModule::PerceptionModule(SpellBook *spellBook) : Module(spellBook, 30)
{
    #ifdef USE_UNSW
    InitManager::GetBlackboard()->thread.configCallbacks["perception"];
    perception = new rUNSWiftPerceptionAdapter();
    #endif
}

PerceptionModule::~PerceptionModule()
{
    #ifdef USE_UNSW
    delete perception;
    #endif
}

void PerceptionModule::OnStart()
{
    #ifdef USE_UNSW
    perception->Start();
    #endif
}

void PerceptionModule::OnStop()
{
    #ifdef USE_UNSW
    perception->Stop();
    #endif
}

void PerceptionModule::Tick(float ellapsedTime)
{
    #ifdef USE_UNSW
    perception->Tick();
    #endif
}