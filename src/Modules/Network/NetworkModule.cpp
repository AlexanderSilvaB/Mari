#include "NetworkModule.h"

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
}

void NetworkModule::OnStop()
{
    delete sock;
}

void NetworkModule::Tick(float ellapsedTime)
{
    if(sock->wait())
    {
        cout << "Connected" << endl;
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