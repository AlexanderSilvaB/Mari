#include "StrategyModule.h"
#include <boost/bind.hpp>
#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

StrategyModule::StrategyModule(SpellBook *spellBook)
    : Module(spellBook, "Strategy", 30)
{
    touch = NULL;
    gameController = new GameController(this->spellBook);   
    safetyMonitor = new SafetyMonitor(this->spellBook);
    potentialFields = new PotentialFields(this->spellBook);

    squareStep = 1;
    squareX = squareY = 0;
    squareL = 2.0f;
    squareTimer = 0;
    circleRadius = 2.0f;
}

StrategyModule::~StrategyModule()
{
    delete gameController;
    delete safetyMonitor;
    delete potentialFields;
    touch = NULL;
}

void StrategyModule::OnStart()
{
    boost::program_options::variables_map config = InitManager::GetBlackboard()->config;
    touch = (Touch *)new AgentTouch();
    touch->readOptions(config); 
    
    gameController->OnStart();
    safetyMonitor->OnStart();
}

void StrategyModule::OnStop()
{
    gameController->OnStop();
    safetyMonitor->OnStop();
    delete touch;
}

void StrategyModule::Load()
{
    LOAD(motion)
    LOAD(perception)
    LOAD(strategy)
    LOAD(behaviour)
}

void StrategyModule::Save()
{
    SAVE(motion)
    SAVE(perception)
    SAVE(strategy)
    SAVE(behaviour) 
}


void StrategyModule::Tick(float ellapsedTime)
{
    SensorValues sensor = touch->getSensors(kinematics);

    safetyMonitor->Tick(ellapsedTime, sensor);
    gameController->Tick(ellapsedTime, sensor);

    cout << "Ball: " << spellBook->perception.vision.ball.BallDistance << ", " << Rad2Deg(spellBook->perception.vision.ball.BallAzimuth) << "º" << endl;
    cout << "Localization: " << spellBook->perception.vision.localization.X << ", " << spellBook->perception.vision.localization.Y << ", " << Rad2Deg(spellBook->perception.vision.localization.Theta) << "º" << endl;

    if(!spellBook->strategy.Started)
    {
        spellBook->motion.Stiff = false;
        spellBook->motion.Stand = false;
        spellBook->motion.Walk = false;
        return;
    }

    if(spellBook->strategy.Penalized)
    {
        spellBook->motion.Stiff = true;
        spellBook->motion.Stand = false;
        spellBook->motion.Walk = false;
        return;
    }

    spellBook->motion.Dead = spellBook->strategy.Die;
    if(spellBook->strategy.Die)
    {
        return;
    }
    spellBook->motion.TipOver = spellBook->strategy.TurnOver;
    if(spellBook->strategy.TurnOver)
    {
        return;
    }
    spellBook->motion.GetupBack = spellBook->strategy.FallenBack;
    if(spellBook->strategy.FallenBack)
    {
        return;
    }
    spellBook->motion.GetupFront = spellBook->strategy.FallenFront;
    if(spellBook->strategy.FallenFront)
    {
        return;
    }

    spellBook->motion.Stiff = true;
    spellBook->motion.Stand = true;
    spellBook->motion.Walk = true;

    if(spellBook->strategy.WalkInSquare)
    {
        if(squareStep == 0)
        {
            squareX += spellBook->motion.Vx * ellapsedTime;
            spellBook->motion.Vx = 0.3f;
            if(squareX > squareL)
            {
                squareStep = 1;
                spellBook->motion.Vx = 0;
            }
        }
        else if(squareStep == 1)
        {
            squareY += spellBook->motion.Vy * ellapsedTime;
            spellBook->motion.Vy = 0.2f;
            if(squareY > squareL)
            {
                squareStep = 2;
                spellBook->motion.Vy = 0;
            }
        }
        else if(squareStep == 2)
        {
            squareX += spellBook->motion.Vx * ellapsedTime;
            spellBook->motion.Vx = -0.3f;
            if(squareX < 0)
            {
                squareStep = 3;
                spellBook->motion.Vx = 0;
            }
        }
        else if(squareStep == 3)
        {
            squareY += spellBook->motion.Vy * ellapsedTime;
            spellBook->motion.Vy = -0.2f;
            if(squareY < 0)
            {
                squareStep = 4;
                spellBook->motion.Vy = 0;
            }
        }
        else if(squareStep == 4)
        {
            squareTimer += ellapsedTime;
            if(squareTimer > 5.0f)
            {
                squareStep = 0;
                squareTimer = 0;
            }
        }
    }
    else if(spellBook->strategy.WalkInCircle)
    {
        spellBook->motion.Vx = 0.3f;
        spellBook->motion.Vth = spellBook->motion.Vx / circleRadius;
    }

    // Nossa estratégia
    //spellBook->motion.HeadYaw = spellBook->perception.vision.ball.BallAzimuth;
    //spellBook->motion.HeadPitch = spellBook->perception.vision.ball.BallElevation;
    //spellBook->motion.HeadSpeed = spellBook->perception.vision.ball.HeadSpeed;
    //spellBook->motion.HeadRelative = spellBook->perception.vision.ball.HeadRelative;
    //spellBook->motion.Vx = 0.1f;
    //spellBook->motion.Vth = 0;

    spellBook->strategy.WalkForward = true;
    spellBook->strategy.TargetX = 0;
    spellBook->strategy.TargetY = 0;
    spellBook->strategy.TargetTheta = 0;

    potentialFields->Tick(ellapsedTime);
}
