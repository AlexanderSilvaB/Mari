#include "NetworkModule.h"
#include "Core/Utils/ImageMessage.h"
#include "Core/Utils/CameraSettingMessage.h"

int NetworkModule::outSize = 0;
char NetworkModule::outData[MAX_SIZE];
sem_t NetworkModule::mutex; 
bool NetworkModule::connected = false;

NetworkModule::NetworkModule(SpellBook *spellBook) : Module(spellBook, "Network", 100)
{
    sem_init(&mutex, 0, 1); 
    infoData[0] = 'R';
    infoData[1] = 'I';
    infoData[2] = 'N';
    infoData[3] = 'O';
    inSize = 0;

}

NetworkModule::~NetworkModule()
{
    sem_destroy(&mutex); 
}

void NetworkModule::OnStart()
{
    sock = new TcpUdpSocket(SERVER_PORT, "", false, false, true, true, 200);
    gcsock = new TcpUdpSocket(GAMECONTROLLER_PORT, "255.255.255.255", true, true, true, false, 200);
    gcsockReturn = new TcpUdpSocket(GAMECONTROLLER_PORT, "255.255.255.255", true, true, true, false, 200);
}

void NetworkModule::OnStop()
{
    delete sock;
    delete gcsock;
    delete gcsockReturn;
}

void NetworkModule::Load()
{
    LOAD(network)
    LOAD(behaviour)
}

void NetworkModule::Save()
{
    SAVE(network)
}

void NetworkModule::Tick(float ellapsedTime)
{
    // Supervisório
    if(sock->wait())
    {
        spellBook->network.TCPConnected = true;
        sem_wait(&mutex);
        if(outSize > 0)
        {
            ((int*)(infoData + 4))[0] = outSize;
            int n = sock->send(infoData, 8);
            usleep(1000*10);
            n = sock->send(outData, outSize);
            if(n < outSize)
                sock->disconnect();
            outSize = 0;
        }
        sem_post(&mutex);
        int sz = sock->receive(data, MAX_SIZE);
        if(sz == 0)
            sock->disconnect();
        else if(sz > 0)
        {
            process:
            if(inSize == 0)
            {
                if(data[0] == 'R' && data[1] == 'I' && data[2] == 'N' && data[3] == 'O')
                {
                    inSize = ((int*)(data + 4))[0];
                    sz -= 8;
                    memcpy(data, data+8, sz);
                    dataSize = 0;
                }
            }
            if(dataSize < inSize && sz > 0)
            {
                int rm = 0;
                for(int i = 0; i < sz && dataSize < inSize; i++)
                {
                    inData[dataSize] = data[i];
                    rm++;
                    dataSize++;
                }
                sz -= rm;
                memcpy(data, data+rm, sz);
            }
            if(dataSize == inSize)
            {
                Process(inSize);
                inSize = 0;
            }
            if(sz > 0)
                goto process;
        }
    }
    else
    {
        spellBook->network.TCPConnected = false;
    }
    connected = spellBook->network.TCPConnected;

    // GameController
    {
        cout << "GC Connected" << endl;
        timeSinceLastGCData += ellapsedTime;
        if(timeSinceLastGCData > 1.0f)
        {
            timeSinceLastGCData = 0;
            gcReturnData.team = spellBook->behaviour.TeamNumber;
            gcReturnData.player = spellBook->behaviour.Number;
            if(!spellBook->behaviour.Started)
            {
                gcReturnData.message = GAMECONTROLLER_RETURN_MSG_MAN_UNPENALISE;
            }
            else
            {
                if(spellBook->behaviour.Penalized)
                {
                    gcReturnData.message = GAMECONTROLLER_RETURN_MSG_MAN_PENALISE;
                }
                else
                {
                    gcReturnData.message = GAMECONTROLLER_RETURN_MSG_ALIVE;
                }
            }
            memcpy (outDataGC, &gcReturnData, sizeof(gcReturnData));
            outSizeGC = sizeof(gcReturnData);
        }
        if(outSizeGC > 0)
        {
            gcsockReturn->send(outDataGC, outSizeGC);
            outSizeGC = 0;
        }
        int inSizeGC = gcsock->receive(inDataGC, MAX_GC_MSG);
        cout << inSizeGC << endl;
        if(inSizeGC > 0)
        {
            cout << "Before memcpy" << endl;
            //Tratar a mensagem do GC aqui
            memcpy (&gcData, inDataGC, inSizeGC);
            cout << "Packet Number: " << (int)gcData.packetNumber << endl;
        }
    }
}

void NetworkModule::Process(int inSize)
{
    vector<char> data(inSize);
    memcpy(data.data(), inData, inSize);

    Message message;
    int sz = message.decode(data);
    if(sz == 0)
        cout << "Invalid message received" << endl;
    else
    {
        switch (message.getType())
        {
            case TYPE_CAM_SETTING:
            {
                CameraSettingMessage camMessage;
                camMessage.decode(data);
                ProcessCameraSetting(camMessage);
            }
                break;
            default:
                break;
        }
    }
}

void NetworkModule::ProcessCameraSetting(CameraSettingMessage &setting)
{
    //cout << "Cam setting: " << setting.getSetting() << " = " << setting.getValue() << endl;
    switch(setting.getSetting())
    {
        case SETTING_BRIGHTNESS:
        {
            switch(spellBook->network.SelectedCamera)
            {
                case 0:
                   spellBook->network.botSettings.brightness = setting.getValue();
                   spellBook->network.botSettings.brightnessChanged = true;
                   break;
                case 1:
                   spellBook->network.topSettings.brightness = setting.getValue();
                   spellBook->network.topSettings.brightnessChanged = true;
                   break;
                default:
                    spellBook->network.botSettings.brightness = setting.getValue();
                    spellBook->network.botSettings.brightnessChanged = true;
                    spellBook->network.topSettings.brightness = setting.getValue();
                    spellBook->network.topSettings.brightnessChanged = true;
                    break;
            }
        }
        break;
        case SETTING_SATURATION:
        {
            switch(spellBook->network.SelectedCamera)
            {
                case 0:
                   spellBook->network.botSettings.saturation = setting.getValue();
                   spellBook->network.botSettings.saturationChanged = true;
                   break;
                case 1:
                   spellBook->network.topSettings.saturation = setting.getValue();
                   spellBook->network.topSettings.saturationChanged = true;
                   break;
                default:
                    spellBook->network.botSettings.saturation = setting.getValue();
                    spellBook->network.botSettings.saturationChanged = true;
                    spellBook->network.topSettings.saturation = setting.getValue();
                    spellBook->network.topSettings.saturationChanged = true;
                    break;
            }
        }
        break;
        case SETTING_CONTRAST:
        {
            switch(spellBook->network.SelectedCamera)
            {
                case 0:
                   spellBook->network.botSettings.contrast = setting.getValue();
                   spellBook->network.botSettings.contrastChanged = true;
                   break;
                case 1:
                   spellBook->network.topSettings.contrast = setting.getValue();
                   spellBook->network.topSettings.contrastChanged = true;
                   break;
                default:
                    spellBook->network.botSettings.contrast = setting.getValue();
                    spellBook->network.botSettings.contrastChanged = true;
                    spellBook->network.topSettings.contrast = setting.getValue();
                    spellBook->network.topSettings.contrastChanged = true;
                    break;
            }
        }
        break;
        case SETTING_SHARPNESS:
        {
            switch(spellBook->network.SelectedCamera)
            {
                case 0:
                   spellBook->network.botSettings.sharpness = setting.getValue();
                   spellBook->network.botSettings.sharpnessChanged = true;
                   break;
                case 1:
                   spellBook->network.topSettings.sharpness = setting.getValue();
                   spellBook->network.topSettings.sharpnessChanged = true;
                   break;
                default:
                    spellBook->network.botSettings.sharpness = setting.getValue();
                    spellBook->network.botSettings.sharpnessChanged = true;
                    spellBook->network.topSettings.sharpness = setting.getValue();
                    spellBook->network.topSettings.sharpnessChanged = true;
                    break;
            }
        }
        break;
        case SETTING_NUMBER:
        {
            spellBook->network.SelectedCamera = setting.getValue();
        }
        break;
        case SETTING_SAVE:
        {
            cout << "Save" << endl;
        }
        break;
        case SETTING_DISCARD:
        {
            cout << "Discard" << endl;
        }
        break;
        default:
        break;
    }
}

bool NetworkModule::SendMessage(Message *message)
{
    if(!connected)
        return false;
    sem_wait(&mutex);
    if(outSize > 0)
    {
        sem_post(&mutex);
        return false;
    }
    vector<char> data;
    message->encode(data);
    char *ptr = data.data();
    memcpy(outData, ptr, data.size());
    outSize = data.size();
    sem_post(&mutex);
    return true;
}

bool NetworkModule::IsConnected()
{
    return connected;
}