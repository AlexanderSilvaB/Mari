#ifndef _NETWORKMODULE_H_
#define _NETWORKMODULE_H_

#include "Core/Module.h"
#include "Core/External/TcpUdpSocket/TcpUdpSocket.h"
#include "Core/Utils/Message.h"

using namespace std;

#define SERVER_PORT 9572
#define MAX_SIZE 922624

class NetworkModule : public Module
{
    private:
        TcpUdpSocket *sock;
        char inData[MAX_SIZE];
        static char outData[MAX_SIZE];
        static int outSize;
    public:
        NetworkModule(SpellBook *spellBook);
        ~NetworkModule();
        void OnStart();
        void OnStop();
        void Tick(float ellapsedTime);
        static bool SendMessage(Message *message);
};

#endif
