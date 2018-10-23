#include "PerceptionModule.h"
#include "rUNSWiftPerceptionAdapter.h"
#include "Core/InitManager.h"

PerceptionModule::PerceptionModule(SpellBook *spellBook) : Module(spellBook, "Perception", 30)
{
    vision = new Vision(this->spellBook);
    InitManager::GetBlackboard()->thread.configCallbacks["perception"];
    perception = new rUNSWiftPerceptionAdapter();
}

PerceptionModule::~PerceptionModule()
{
    delete vision;
    delete perception;
}

void PerceptionModule::OnStart()
{
    perception->Start();
}

void PerceptionModule::OnStop()
{
    perception->Stop();
}

void PerceptionModule::Load()
{
    LOAD(perception);
}

void PerceptionModule::Save()
{
    SAVE(perception);
}

void PerceptionModule::Tick(float ellapsedTime)
{
    vision->Tick(ellapsedTime);
    perception->Tick();
}