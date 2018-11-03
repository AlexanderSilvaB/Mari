#include "NetworkModule.h"
#include "Core/External/unsw/unsw/gamecontroller/RoboCupGameControlData.hpp"
#include "SPLStandardMessage.h"
#include <cstring>

int NetworkModule::outSize = 0;
char NetworkModule::outData[MAX_SIZE];

NetworkModule::NetworkModule(SpellBook *spellBook) : Module(spellBook, "Network", 100)
{
    
}

NetworkModule::~NetworkModule()
{
    
}

void NetworkModule::OnStart()
{
    sock = new TcpUdpSocket(SERVER_PORT, "", false, false, true, true, 200);
    gcsock = new TcpUdpSocket(GAMECONTROLLER_DATA_PORT, "255.255.255.255", true, true, true, false, 200);
}

void NetworkModule::OnStop()
{
    delete sock;
    delete gcsock;
}

void NetworkModule::Tick(float ellapsedTime)
{
    if(sock->wait())
    {
        cout << "TCP Connected" << endl;
        if(outSize > 0)
        {
            int n = sock->send(outData, outSize);
            if(n < outSize)
                sock->disconnect();
            outSize = 0;
        }
        int inSize = sock->receive(inData, MAX_SIZE);
        if(inSize == 0)
            sock->disconnect();
        else if(inSize > 0)
        {

        }
    }
    //if(gcsock->wait())
    {
        cout << "GC Connected" << endl;
        if(outSizeGC > 0)
        {
            gcsock->send(outDataGC, outSizeGC);
            outSizeGC = 0;
        }
        int inSizeGC = gcsock->receive(inDataGC, MAX_GC_MSG);
        cout << inSizeGC << endl;
        if(inSizeGC > 0)
        {
            cout << "Before memcpy" << endl;
            //Tratar a mensagem do GC aqui
            memcpy (&gcData, inDataGC, inSizeGC);
            cout << "Packet Number: " << (int)gcData.state << endl;
        }
    }
}

bool NetworkModule::SendMessage(Message *message)
{
    if(outSize > 0)
        return false;
    vector<char> data;
    message->encode(data);
    char *ptr = data.data();
    memcpy(outData, ptr, data.size());
    outSize = data.size();
    return true;
}