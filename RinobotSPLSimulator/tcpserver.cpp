#include "tcpserver.h"
#include <QDebug>
#include "../supervisorio/message.h"
#include "../supervisorio/imagemessage.h"
#include "../supervisorio/camerasettingmessage.h"
#include "../supervisorio/logger.h"

TCPServer::TCPServer(int port)
{
    this->port = port;
    running = true;
    client = nullptr;
    is_connected = false;
    cameraParameters[0] = cameraParameters[1] = cameraParameters[2] = cameraParameters[3] = 1.0f;
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(this, SIGNAL(sendMessageEmit(int)), this, SLOT(sendMessage(int)));
    server->listen(QHostAddress::Any, port);
    start();
    Logger::log("Listening on port "+QString::number(port));
}

TCPServer::~TCPServer()
{
    stop();
    running = false;
    quit();
    wait();
    delete server;
}

void TCPServer::stop()
{
    if(client != nullptr)
    {
        Logger::log("Disconnected from "+client->localAddress().toString());
        client->disconnectFromHost();
        is_connected = false;
    }
}

void TCPServer::newConnection()
{
    client = server->nextPendingConnection();
    Logger::log("Connected to "+client->localAddress().toString());
    is_connected = true;
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));

    sendCameraParameters();
}

void TCPServer::disconnected()
{
    Logger::log("Disconnected from "+client->localAddress().toString());
    is_connected = false;
    client = nullptr;
}

void TCPServer::sendCameraParameters()
{
    CameraSettingMessage message1(SETTING_BRIGHTNESS, cameraParameters[0]*100);
    QByteArray data1;
    message1.encode(data1);
    client->write(data1);
    QThread::msleep(50);

    CameraSettingMessage message2(SETTING_SATURATION, cameraParameters[1]*100);
    QByteArray data2;
    message2.encode(data2);
    client->write(data2);
    QThread::msleep(50);

    CameraSettingMessage message3(SETTING_CONTRAST, cameraParameters[2]*100);
    QByteArray data3;
    message3.encode(data3);
    client->write(data3);
    QThread::msleep(50);

    CameraSettingMessage message4(SETTING_SHARPNESS, cameraParameters[3]*100);
    QByteArray data4;
    message4.encode(data4);
    client->write(data4);
    QThread::msleep(50);
}

void TCPServer::sendMessage(int msgType)
{
    switch (msgType)
    {
        case TYPE_BASIC:
        {
            QString str("Random number: ");
            str += QString::number(rand());
            Message message(str);
            message.setLevel((rand() % 4)+1);
            QByteArray data;
            message.encode(data);
            client->write(data);
            //Logger::log("Sending: "+message.toString());
        }
            break;
        case TYPE_IMAGE:
        {
            int tp = rand() % 5;
            QString str("Type ");
            str += QString::number(tp);
            ImageMessage imageMessage(str, 320, 240, tp);
            float val = (cameraParameters[0] + cameraParameters[1]+ cameraParameters[2]+ cameraParameters[3]) / 4.0f;
            uchar *ptr = imageMessage.getData();
            for(int i = 0; i < imageMessage.getDataSize(); i++)
            {
                ptr[i] = (uchar)((rand() % 256) * val);
            }
            QByteArray data;
            imageMessage.encode(data);
            client->write(data);
            //Logger::log("Sending: "+imageMessage.toString());
        }
            break;
        default:
            break;
    }
}

void TCPServer::readyRead()
{
    QByteArray data = client->readAll();
    Message message;
    int skipSize = message.decode(data);
    if(skipSize == 0)
        Logger::log("Invalid message received");
    else
    {
        switch (message.getType())
        {
            case TYPE_CAM_SETTING:
            {
                CameraSettingMessage message;
                message.decode(data);
                if(message.getSetting() >= 0 && message.getSetting() < 4)
                {
                    cameraParameters[message.getSetting()] = message.getValue() / 100.0f;
                }
                else if(message.getSetting() == SETTING_SAVE)
                {
                    sendCameraParameters();
                }
                else if(message.getSetting() == SETTING_DISCARD)
                {
                    cameraParameters[0] = cameraParameters[1] = cameraParameters[2] = cameraParameters[3] = 1.0f;
                    sendCameraParameters();
                }
                Logger::log("Received: " + message.toString());
            }
                break;
            default:
                Logger::log("Received: " + message.toString());
                break;
        }
    }
}


void TCPServer::run()
{
    while(running)
    {
        while(is_connected)
        {
            int msgType = rand() % 2;
            switch (msgType)
            {
                case 0:
                    msgType = TYPE_BASIC;
                    break;
                case 1:
                    msgType = TYPE_IMAGE;
                    break;
                default:
                    break;
            }
            emit sendMessageEmit(msgType);
            QThread::msleep(30);
        }
        QThread::msleep(1000);
    }
}
