#ifndef _NETWORKMODULE_H_
#define _NETWORKMODULE_H_

#include "Core/Module.h"
#include "Core/External/TcpUdpSocket/TcpUdpSocket.h"
#include "Core/Utils/Message.h"
#include <iostream>

using namespace std;

#define SERVER_PORT 9572
#define MAX_SIZE 922624
#define MAX_GC_MSG 1024

class NetworkModule : public Module
{
    private:
        TcpUdpSocket *sock;
        TcpUdpSocket *gcsock, *gcsockReturn;

        char inData[MAX_SIZE];
        static char outData[MAX_SIZE];
        static int outSize;

        char outDataGC[MAX_GC_MSG];
        char inDataGC[MAX_GC_MSG];
        int outSizeGC;
        float timeSinceLastGCData;

        struct RoboCupGameControlData gcData;
        struct RoboCupGameControlReturnData gcReturnData;

    public:
        NetworkModule(SpellBook *spellBook);
        ~NetworkModule();
        void OnStart();
        void OnStop();
        void Tick(float ellapsedTime);
        static bool SendMessage(Message *message);
        void Load();
        void Save();
};

#endif
