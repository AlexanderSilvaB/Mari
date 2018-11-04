#ifndef _NETWORKMODULE_H_
#define _NETWORKMODULE_H_

#include "Core/Module.h"
#include "Core/External/TcpUdpSocket/TcpUdpSocket.h"
#include "Core/Utils/Message.h"
#include "Core/Utils/CameraSettingMessage.h"
#include <semaphore.h> 
#include "Core/Utils/CombinedCamera.hpp"
#include "Core/GC/RoboCupGameControlData.h"
#include "Core/GC/SPLStandardMessage.h"

using namespace std;

#define SERVER_PORT 9572
#define MAX_SIZE 922624

//Game Controller
#define MAX_GC_MSG 1024

class NetworkModule : public Module
{
    private:
        static sem_t mutex; 
        static bool connected;
        TcpUdpSocket *sock;
        char inData[MAX_SIZE];
        char data[MAX_SIZE];
        int inSize, dataSize;
        char infoData[8];
        static char outData[MAX_SIZE];
        static int outSize;
        void Process(int inSize);
        void ProcessCameraSetting(CameraSettingMessage &setting);
        void setControl(Camera *camera, const uint32_t controlId, const int32_t controlValue);

        //Game Controller   
        TcpUdpSocket *gcsock, *gcsockRet;
        char outDataGC[MAX_GC_MSG];
        char inDataGC[MAX_GC_MSG];
        int outSizeGC;
        float timeSinceLastGCReturn;
        float timeSinceLastGCPlayerData;

        struct RoboCupGameControlData gcData;
        struct SPLStandardMessage gcSPLData;
        struct RoboCupGameControlReturnData gcRetData;

    public:
        NetworkModule(SpellBook *spellBook);
        ~NetworkModule();
        void OnStart();
        void OnStop();
        void Load();
        void Save();
        void Tick(float ellapsedTime);
        static bool SendMessage(Message *message);
        static bool IsConnected();
};

#endif
