#include "StrategyModule.h"
#include <boost/bind.hpp>
#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

StrategyModule::StrategyModule(SpellBook *spellBook)
    : Module(spellBook, "Strategy", 30)
{
    gameController = new GameController(this->spellBook);   
    safetyMonitor = new SafetyMonitor(this->spellBook);
    potentialFields = new PotentialFields(this->spellBook);
    headController = new HeadController(this->spellBook);
    ballTracker = new BallTracker(this->spellBook);

    squareStep = 1;
    squareX = squareY = 0;
    squareL = 2.0f;
    squareTimer = 0;
    circleRadius = 2.0f;

    goalie = new GoalieRole(this->spellBook);
    defender = new DefenderRole(this->spellBook);
    rinoplayer = new RinoPlayer(this->spellBook);
}

StrategyModule::~StrategyModule()
{
    delete gameController;
    delete safetyMonitor;
    delete potentialFields;
    delete headController;

    delete ballTracker;

    delete goalie;
}

void StrategyModule::OnStart()
{
    gameController->OnStart();
    safetyMonitor->OnStart();
    ballTracker->OnStart();
}

void StrategyModule::OnStop()
{
    gameController->OnStop();
    safetyMonitor->OnStop();
    ballTracker->OnStop();
}

void StrategyModule::Load()
{
    //LOAD(motion)
    LOAD(perception)
    LOAD(strategy)
    LOAD(behaviour)
}

void StrategyModule::Save()
{
    SAVE(motion)
    //SAVE(perception)
    SAVE(strategy)
    SAVE(behaviour) 
}


void StrategyModule::Tick(float ellapsedTime)
{
    //cout << "Strategy" << endl;
    Blackboard *blackboard = InitManager::GetBlackboard();
    const SensorValues &sensor = readFrom(motion, sensors);

    safetyMonitor->Tick(ellapsedTime, sensor);
    gameController->Tick(ellapsedTime, sensor);

    if(spellBook->motion.Calibrate)
    {
        spellBook->motion.Stiff = true;
        spellBook->motion.Stand = true;
        spellBook->motion.Walk = false;
        cout << endl << endl << "Calibrate" << endl;
        cout << "gyrXOffset=" << spellBook->motion.GyroX << endl;
        cout << "gyrYOffset=" << spellBook->motion.GyroY << endl;
        cout << "angleXOffset=" << spellBook->motion.AngleX << endl;
        cout << "angleYOffset=" << spellBook->motion.AngleY << endl;
        cout << "# Save this to /home/nao/data/configs/robotName.cfg" << endl;
        cout << endl << endl;
        return;
    }

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
        return;
    }
    else if(spellBook->strategy.WalkInCircle)
    {
        spellBook->motion.Vx = 0.3f;
        spellBook->motion.Vth = spellBook->motion.Vx / circleRadius;
        return;
    }

    ballTracker->Tick(ellapsedTime, sensor);

    switch(spellBook->behaviour.Number)
    {
        case 1:
            goalie->Tick(ellapsedTime, sensor);
            break;
        case 2:
            defender->Tick(ellapsedTime, sensor);
            break;
        case 3:
            rinoplayer->Tick(ellapsedTime, sensor);
            break;
        default:
            break;
    }
        

    //potentialFields->Tick(ellapsedTime);
    headController->Tick(ellapsedTime, sensor);
}
