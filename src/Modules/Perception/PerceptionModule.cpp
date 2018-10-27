#include "PerceptionModule.h"
#include "rUNSWiftPerceptionAdapter.h"
#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

PerceptionModule::PerceptionModule(SpellBook *spellBook) : Module(spellBook, "Perception", 30)
{
    //SetPriority(20);
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
    //if(spellBook->perception.vision.Enabled)
    //    vision->Tick(ellapsedTime);
    cout << "Perception->Tick" << endl;
    perception->Tick();
    Blackboard *blackboard = InitManager::GetBlackboard();
    AbsCoord robotPos = readFrom(localisation, robotPos);
    AbsCoord ballPos = readFrom(localisation, ballPos);
    RRCoord ballPosRR = readFrom(localisation, ballPosRR);
    AbsCoord ballPosRRC = readFrom(localisation, ballPosRRC);
    uint32_t balls = readFrom(vision, balls).size();

    int numFieldLinePoints = readFrom(vision, numFieldLinePoints);
    cout << "NUm field line: " << numFieldLinePoints << endl; 

    spellBook->perception.vision.localization.X = robotPos.x() * 0.001f;
    spellBook->perception.vision.localization.Y = robotPos.y() * 0.001f;
    spellBook->perception.vision.localization.Theta = robotPos.theta();


    
    spellBook->perception.vision.ball.BallDetected = balls > 0;
    spellBook->perception.vision.ball.BallAzimuth = ballPosRR.heading();
    spellBook->perception.vision.ball.BallElevation = Rad2Deg(17.0f);
    spellBook->perception.vision.ball.BallDistance = ballPosRR.distance() * 0.001f;
    spellBook->perception.vision.ball.HeadSpeed = 0.2f;
    spellBook->perception.vision.ball.HeadRelative = true;
}