#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

class TCPServer : public QThread
{
    Q_OBJECT

    private:
        bool running;
        bool is_connected;
        int port;
        QTcpServer *server;
        QTcpSocket *client;
        float cameraParameters[4];
        void sendCameraParameters();

    public:
        TCPServer(int port);
        ~TCPServer();

        void run() override;

        void stop();
    private slots:
        void newConnection();
        void sendMessage(int msgType);
        void disconnected();
        void readyRead();
    signals:
        void sendMessageEmit(int msgType);
};
#endif // TCPSERVER_H
