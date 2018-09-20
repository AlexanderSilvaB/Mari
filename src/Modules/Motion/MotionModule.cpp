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

    spellBook->motionSpell.HeadAzimuth = 0;
    headAngles[0] = headAngles[1] = 0;
    nextHeadAngles[0] = nextHeadAngles[1] = 0;

    minDistanceToBall = 0.20f;

    vx = vy = vth = 0;
    headSpeed = 0;
    _stiff = false;
    _stand = false;

    stiff = true;
    stand = true;
    kickLeft = kickRight = false;
    limpLeft = limpRight = false;
    getupFront = getupBack = false;
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
    
    if(spellBook->motionSpell.RemoteMotion)
    {
        vx = spellBook->motionSpell.RemoteVx;
        vy = spellBook->motionSpell.RemoteVy;
        vth = spellBook->motionSpell.RemoteVth;
        nextHeadAngles[0] = spellBook->motionSpell.RemoteHeadYaw;
        nextHeadAngles[1] = spellBook->motionSpell.RemoteHeadPitch;
        kickLeft = spellBook->motionSpell.RemoteKickLeft;
        kickRight = spellBook->motionSpell.RemoteKickRight;
        limpLeft = spellBook->motionSpell.RemoteLimpLeft;
        limpRight = spellBook->motionSpell.RemoteLimpRight;
        stand = spellBook->motionSpell.RemoteStand;
        stiff = spellBook->motionSpell.RemoteStiff;
    }
    else
    {
        // Head
        if(spellBook->visionSpell.BallDetected)
        {
            #ifdef USE_UNSW
            SensorValues sensors = motion->GetSensors();
            //cout << sensors.joints.angles[Joints::HeadYaw] << endl;
            //cout << sensors.joints.angles[Joints::HeadPitch] << endl;
            //headAngles[0] = sensors.joints.angles[Joints::HeadYaw];
            headAngles[0] = 0;
            //headAngles[1] = sensors.joints.angles[Joints::HeadPitch];
            headAngles[1] = 0;
            #else
            #ifdef USE_QIBUILD
            headAngles[0] = motion->GetAngle("HeadYaw", true);
            headAngles[1] = motion->GetAngle("HeadPitch", true);
            #endif
            #endif

            headSpeed = spellBook->visionSpell.HeadSpeed;
            
            nextHeadAngles[0] = headAngles[0] + spellBook->visionSpell.BallAzimuth;
            //nextHeadAngles[0] = spellBook->visionSpell.BallAzimuth;
            //nextHeadAngles[1] = headAngles[1] + spellBook->visionSpell.BallElevation + Deg2Rad(13.75f);
            nextHeadAngles[1] = headAngles[1] + spellBook->visionSpell.BallElevation;
            //nextHeadAngles[1] = spellBook->visionSpell.BallElevation;

            spellBook->motionSpell.HeadAzimuth = spellBook->visionSpell.BallAzimuth;
        }
        else
        {
            if(spellBook->visionSpell.TimeSinceBallSeen > 0.3f)
            {
                nextHeadAngles[0] = 0.0f;
                nextHeadAngles[1] = 0.0f;

                spellBook->motionSpell.HeadAzimuth = 0;
            }
            else
            {
                nextHeadAngles[0] = spellBook->visionSpell.BallAzimuth;
                nextHeadAngles[1] = spellBook->visionSpell.BallElevation;
            }
        }
        headRelative = spellBook->visionSpell.HeadRelative;

        // Walk
        if(spellBook->visionSpell.BallDetected)
        {
            if(spellBook->visionSpell.BallDistance > minDistanceToBall)
            {
                cout << "Walk: " << Rad2Deg(spellBook->visionSpell.BallAzimuth) << ", " << spellBook->visionSpell.BallDistance << endl;
                if(spellBook->visionSpell.BallDistance > minDistanceToBall*1.3f)
                    vx = 0.3f;
                else
                    vx = 0.3f;//min(spellBook->visionSpell.BallDistance, 0.1f);
                vy = 0.0f;
                if(abs(spellBook->motionSpell.HeadAzimuth) > Deg2Rad(10.0f))
                {
                    vth = spellBook->motionSpell.HeadAzimuth * 0.6f;
                }
                else
                    vth = 0.0f;
            }
            else
            {
                cout << "Turn: " << Rad2Deg(spellBook->visionSpell.BallAzimuth) << ", " << spellBook->visionSpell.BallDistance << endl;
                vx = 0.0f;
                vy = 0.0f;
                if(abs(spellBook->motionSpell.HeadAzimuth) > Deg2Rad(10.0f))
                {
                    vth = spellBook->motionSpell.HeadAzimuth * 0.6f;
                }
                else
                    vth = 0.0f;
            }
        }
        else
        {
            cout << "Search: " << spellBook->visionSpell.TimeSinceBallSeen << endl;
            if(spellBook->visionSpell.TimeSinceBallSeen > 0.3f)
            {
                vx = 0.3f;
                vy = 0.0f;
                vth = 0.0f;
            }
            if(spellBook->visionSpell.TimeSinceBallSeen > 1.5f)
            {
                vth = Deg2Rad(10.0f);
            }
        }
    }  

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
    else
    {
        request.head = ActionCommand::Head(nextHeadAngles[0], nextHeadAngles[1], !headRelative, headSpeed, 0.2f);
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
        anglesList[0] = nextHeadAngles[0];
        anglesList[1] = nextHeadAngles[1];
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