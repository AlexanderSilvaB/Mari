#include "MotionModule.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/SimpleTimer.h"
#include <iostream>
#include "Core/InitManager.h"

using namespace std;

MotionModule::MotionModule(SpellBook *spellBook) : Module(spellBook, 0)
{
    this->highPriority = true;
    #ifdef USE_UNSW
    InitManager::GetBlackboard()->thread.configCallbacks["motion"];
    motion = new rUNSWiftMotionAdapter();
    #else
    #ifdef USE_QIBUILD
    motion = Robot::CreateModule<Motion>();
    posture = Robot::CreateModule<Posture>();
    moveConfig["MaxStepX"] = 0.020f;
    moveConfig["StepHeight"] = 0.01f;
    namesList.push_back("HeadYaw");
    namesList.push_back("HeadPitch");
    anglesList.push_back(0.0f);
    anglesList.push_back(0.0f);
    #endif
    #endif

    headYaw = headPitch = 0;

    minDistanceToBall = 0.20f;

    vx = vy = vth = 0;
    headSpeed = 0;
    _stiff = false;
    _stand = false;

    stiff = false;
    stand = false;
    kickLeft = kickRight = false;
    limpLeft = limpRight = false;
    getupFront = getupBack = false;
    walk = false;
}

MotionModule::~MotionModule()
{
    #ifdef USE_UNSW
    delete motion;
    #endif
}

void MotionModule::OnStart()
{
    #ifdef USE_UNSW
    motion->Start();
    #else
    #ifdef USE_QIBUILD
    posture->GoToPosture("StandInit", 1.0f);
    motion->Move(0.01f, 0.0f, 0.0f);
    usleep(200*1000);
    motion->Move(0.0f, 0.0f, 0.0f);
    #endif
    #endif
}

void MotionModule::OnStop()
{
    #ifdef USE_UNSW
    motion->Stop();
    #else
    #ifdef USE_QIBUILD
    motion->KillAll();
    posture->GoToPosture("Crouch", 1.0f);
    motion->Rest();
    #endif
    #endif
}

void MotionModule::Tick(float ellapsedTime)
{
    vx = spellBook->motionSpell.Vx;
    vy = spellBook->motionSpell.Vy;
    vth = spellBook->motionSpell.Vth;
    headYaw = spellBook->motionSpell.HeadYaw;
    headPitch = spellBook->motionSpell.HeadPitch;
    headSpeed = spellBook->motionSpell.HeadSpeed;
    headRelative = spellBook->motionSpell.HeadRelative;
    kickLeft = spellBook->motionSpell.KickLeft;
    kickRight = spellBook->motionSpell.KickRight;
    limpLeft = spellBook->motionSpell.LimpLeft;
    limpRight = spellBook->motionSpell.LimpRight;
    stand = spellBook->motionSpell.Stand;
    stiff = spellBook->motionSpell.Stiff;
    getupFront = spellBook->motionSpell.GetupFront;
    getupBack = spellBook->motionSpell.GetupBack;
    walk = spellBook->motionSpell.Walk;


    #ifdef USE_UNSW
    ActionCommand::All request;
    if(stiff != _stiff)
    {
        request.stiffen = stiff ? ActionCommand::STIFFEN : ActionCommand::NONE;
        _stiff = stiff;
    }
    else if(stand != _stand)
    {
        request.body = ActionCommand::Body(stand ? ActionCommand::Body::STAND : ActionCommand::Body::INITIAL);
        _stand = stand;
    }
    else if(kickLeft)
    {
        request.body = ActionCommand::Body(ActionCommand::Body::KICK);
        request.body.foot = ActionCommand::Body::LEFT;
    }
    else if(kickRight)
    {
        request.body = ActionCommand::Body(ActionCommand::Body::KICK);
        request.body.foot = ActionCommand::Body::RIGHT;
    }
    else if(getupFront)
    {
        request.body = ActionCommand::Body(ActionCommand::Body::GETUP_FRONT);
        request.body.foot = ActionCommand::Body::RIGHT;
    }
    else if(getupBack)
    {
        request.body = ActionCommand::Body(ActionCommand::Body::GETUP_BACK);
        request.body.foot = ActionCommand::Body::RIGHT;
    }
    else if(walk)
    {
        request.head = ActionCommand::Head(headYaw, headPitch, !headRelative, headSpeed, 0.2f);
        //if(vx != 0 || vy != 0 || vth != 0)
        {
            vx *= 1000.0f;
            vy *= 1000.0f;
            ScaleWalk2014(&vx, &vy, &vth);
            cout << vx << ", " << vy << ", " << Rad2Deg(vth) << endl;
            request.body = ActionCommand::Body(ActionCommand::Body::WALK, vx, vy, vth);
            request.body.bend = 30.0f;
        }
        //else
        //    request.body = ActionCommand::Body();       
        request.body.leftArmLimp = limpLeft;
        request.body.leftArmLimp = limpRight;
    }
    motion->Tick(request);
    #else
    #ifdef USE_QIBUILD
    if(stand != _stand)
    {
        if(stand)
            motion->WakeUp();
        else
            motion->Rest();
        _stand = stand;
    }
    else
    {
        anglesList[0] = headYaw;
        anglesList[1] = headPitch;
        motion->AnglesInterpolation(namesList, anglesList, 0.1f);
        motion->Move(vx, vy, vth, moveConfig);
    }
    #endif
    #endif
}

void MotionModule::ScaleWalk2014(float *forward, float *left, float *turn)
{
    float f = *forward;
    float l = *left;
    float t = *turn;
    float sum = abs(f/300.0f) + abs(l/200.0f) + abs(t/1.5f);
    if(sum <= 1)
        return;
    *forward = round(f/sum);
    *left = round(l/sum);
    *turn = round(t/sum);
}