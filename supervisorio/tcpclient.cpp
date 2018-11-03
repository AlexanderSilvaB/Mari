#include <QAbstractSocket>
#include "tcpclient.h"
#include "logger.h"
#include "camerasettingmessage.h"

TCPClient::TCPClient()
{
    imageType = "";
    port = 9572;
    socket = new QTcpSocket(this);
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
    Message message;
    int skipSize = message.decode(data);
    if(skipSize == 0)
    {
        //Logger::log("Invalid message received");
    }
    else
    {
        switch (message.getType())
        {
            case TYPE_IMAGE:
            {
                ImageMessage imageMessage;
                imageMessage.decode(data);
                processImage(imageMessage);
                //Logger::log("Received: " + imageMessage.toString());
            }
                break;
            case TYPE_CAM_SETTING:
            {
                CameraSettingMessage camMessage;
                camMessage.decode(data);
                emit cameraSetting(camMessage.getSetting(), camMessage.getValue());
            }
                break;
            default:
                emit messageReceived(message);
                //Logger::log("Received: " + message.toString());
                break;
        }
    }
}

