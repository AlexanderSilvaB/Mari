#include <QAbstractSocket>
#include "tcpclient.h"
#include "logger.h"
#include "camerasettingmessage.h"

TCPClient::TCPClient()
{
    imageType = "";
    port = 9572;
    inSize = 0;
    socket = new QTcpSocket(this);
    infoDataRaw[0] = 'R';
    infoDataRaw[1] = 'I';
    infoDataRaw[2] = 'N';
    infoDataRaw[3] = 'O';
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

}

TCPClient::~TCPClient()
{
    delete socket;
}

void TCPClient::setImageType(QString name)
{
    imageType = name;
    Logger::log("Image type: "+imageType);
}

void TCPClient::setPort(int port)
{
    this->port = port;
}

void TCPClient::processImage(ImageMessage &imageMessage)
{
    if(!imageTypes.contains(imageMessage.getName()))
    {
        imageTypes.append(imageMessage.getName());
        emit addImageType(imageMessage.getName());
    }
    if(imageType.isEmpty() || imageType == imageMessage.getName())
    {
        emit updateImage(imageMessage);
    }
}

bool TCPClient::connectToHost(QString address)
{
    if(isConnected())
        disconnectFromHost();
    this->address = address;
    Logger::log("Connecting to "+address+":"+QString::number(port));
    socket->connectToHost(address, port);
    if(!socket->waitForConnected(1000))
    {
        Logger::log("Failed connecting to "+address+":"+QString::number(port)+" -> "+socket->errorString());
        return false;
    }
    return true;
}

bool TCPClient::disconnectFromHost()
{
    if(isConnected())
    {
        Logger::log("Disconnecting from "+address+":"+QString::number(port));
        socket->disconnectFromHost();
    }
    return true;
}

bool TCPClient::isConnected()
{
    return socket->isOpen();
}

bool TCPClient::send(Message *message)
{
    QByteArray data;
    if(message->encode(data) > 0)
    {
        Logger::log("Sending: "+message->toString());
        ((int*)(infoDataRaw + 4))[0] = data.size();
        socket->write(infoDataRaw, 8);
        QThread::msleep(10);
        socket->write(data);
        return true;
    }
    else
    {
        Logger::log("Cannot send: "+message->toString());
    }
    return false;
}

void TCPClient::connected()
{
    Logger::log("Connected to "+address+":"+QString::number(port));
}

void TCPClient::disconnected()
{
    Logger::log("Disconnected from "+address+":"+QString::number(port));
}

void TCPClient::readyRead()
{
    QByteArray data = socket->readAll();
    process:
    if(inSize == 0)
    {
        char *ptr = data.data();
        if(ptr[0] == 'R' && ptr[1] == 'I' && ptr[2] == 'N' && ptr[3] == 'O')
        {
            inSize = ((int*)(ptr + 4))[0];
            qDebug() << "InSize: " << inSize;
            data.remove(0, 8);
        }
    }
    if(inData.size() < inSize && data.size() > 0)
    {
        int rm = 0;
        for(int i = 0; i < data.size() && inData.size() < inSize; i++)
        {
            inData.append(data.at(i));
            rm++;
        }
        data.remove(0, rm);
    }
    if(inData.size() == inSize)
    {
        Message message;
        int skipSize = message.decode(inData);
        if(skipSize == 0)
        {
            Logger::log("Invalid message received");
        }
        else
        {
            switch (message.getType())
            {
                case TYPE_IMAGE:
                {
                    ImageMessage imageMessage;
                    imageMessage.decode(inData);
                    processImage(imageMessage);
                    Logger::log("Received: " + imageMessage.toString());
                }
                    break;
                case TYPE_CAM_SETTING:
                {
                    CameraSettingMessage camMessage;
                    camMessage.decode(inData);
                    emit cameraSetting(camMessage.getSetting(), camMessage.getValue());
                }
                    break;
                default:
                    emit messageReceived(message);
                    Logger::log("Received: " + message.toString());
                    break;
            }
        }
        inData.clear();
        inSize = 0;
    }
    if(data.size() > 0)
        goto process;
}

