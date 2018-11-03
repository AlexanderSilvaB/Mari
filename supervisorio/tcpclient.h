#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QThread>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "message.h"
#include "imagemessage.h"

class TCPClient : public QObject
{
    Q_OBJECT

    private:
        int port;
        QString address;
        QString imageType;
        QTcpSocket *socket;
        QStringList imageTypes;

        void processImage(ImageMessage &imageMessage);
    public:
        TCPClient();
        ~TCPClient();

        void setImageType(QString name);
        void setPort(int port);

        bool connectToHost(QString address);
        bool disconnectFromHost();
        bool isConnected();
        bool send(Message *message);

    public slots:
        void connected();
        void disconnected();
        void readyRead();

    signals:
        void updateImage(ImageMessage imageMessage);
        void addImageType(QString name);
        void cameraSetting(int setting, int value);
        void messageReceived(Message msg);

};

#endif // TCPCLIENT_H
