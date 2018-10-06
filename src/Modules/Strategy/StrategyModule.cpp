#include "StrategyModule.h"
#include <boost/bind.hpp>
#include "Core/InitManager.h"

int step = 0;
float x = 0;
float y = 0;
#define LIM 1.0f
float tmV = 0;

StrategyModule::StrategyModule(SpellBook *spellBook)
    : Module(spellBook, "Strategy", 30)
{
    touch = NULL;
    gameController = new GameController(spellBook);   
    safetyMonitor = new SafetyMonitor(spellBook);
}

StrategyModule::~StrategyModule()
{
    delete gameController;
    delete safetyMonitor;
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

void StrategyModule::Tick(float ellapsedTime)
{
    SensorValues sensor = touch->getSensors(kinematics);

    safetyMonitor->Tick(ellapsedTime, sensor);
    gameController->Tick(ellapsedTime, sensor);

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
        return;
    spellBook->motion.TipOver = spellBook->strategy.TurnOver;
    if(spellBook->strategy.TurnOver)
        return;
    spellBook->motion.GetupBack = spellBook->strategy.FallenBack;
    if(spellBook->strategy.FallenBack)
        return;
    spellBook->motion.GetupFront = spellBook->strategy.FallenFront;
    if(spellBook->strategy.FallenFront)
        return;

    spellBook->motion.Stiff = true;
    spellBook->motion.Stand = true;
    spellBook->motion.Walk = true;

    if(step == 0)
    {
        x += spellBook->motion.Vx * ellapsedTime;
        spellBook->motion.Vx = 0.3f;
        if(x > LIM)
        {
            step = 1;
            spellBook->motion.Vx = 0;
        }
    }
    else if(step == 1)
    {
        y += spellBook->motion.Vy * ellapsedTime;
        spellBook->motion.Vy = 0.3f;
        if(y > LIM)
        {
            step = 2;
            spellBook->motion.Vy = 0;
        }
    }
    else if(step == 2)
    {
        x += spellBook->motion.Vx * ellapsedTime;
        spellBook->motion.Vx = -0.3f;
        if(x < 0)
        {
            step = 3;
            spellBook->motion.Vx = 0;
        }
    }
    else if(step == 3)
    {
        y += spellBook->motion.Vy * ellapsedTime;
        spellBook->motion.Vy = -0.3f;
        if(y < 0)
        {
            step = 4;
            spellBook->motion.Vy = 0;
        }
    }
    else if(step == 4)
    {
        tmV += ellapsedTime;
        if(tmV > 5.0f)
        {
            step = 0;
            tmV = 0;
        }
    }
    
    if(spellBook->perception.ball.BallDetected)
    {
        spellBook->motion.HeadYaw = spellBook->perception.ball.BallAzimuth;
        spellBook->motion.HeadPitch = spellBook->perception.ball.BallElevation;
        spellBook->motion.HeadSpeed = spellBook->perception.ball.HeadSpeed;
        spellBook->motion.HeadRelative = spellBook->perception.ball.HeadRelative;
    }
    else if(spellBook->perception.ball.TimeSinceBallSeen > 2.0f)
    {
        spellBook->motion.HeadYaw = 0.0f;
        spellBook->motion.HeadPitch = 0.0f;
        spellBook->motion.HeadSpeed = spellBook->perception.ball.HeadSpeed;
        spellBook->motion.HeadRelative = spellBook->perception.ball.HeadRelative;
    }
}