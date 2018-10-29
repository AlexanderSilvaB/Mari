#include "NetworkModule.h"

NetworkModule::NetworkModule(SpellBook *spellBook) : Module(spellBook, "Network", 100)
{
    outSize = 0;
}

NetworkModule::~NetworkModule()
{
    
}

void NetworkModule::OnStart()
{
    sock = new TcpUdpSocket(SERVER_PORT, "", false, false, true, true);
}

void NetworkModule::OnStop()
{
    delete sock;
}

void NetworkModule::Tick(float ellapsedTime)
{
    if(sock->wait())
    {
        if(outSize > 0)
        {
            int n = sock->send(outData, outSize);
            if(n < outSize)
                sock->disconnect();
            outSize = 0;
        }
    }
}