#include "PerceptionModule.h"
#include "rUNSWiftPerceptionAdapter.h"
#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

PerceptionModule::PerceptionModule(SpellBook *spellBook) : Module(spellBook, "Perception", 30)
{
    //SetMaxTime(5000);
    vision = new VisionModule(this->spellBook);
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
    cout << "Perception->Tick" << endl;
    if(spellBook->perception.vision.Enabled)
        vision->Tick(ellapsedTime);
    perception->Tick();
}