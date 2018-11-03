#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../supervisorio/logger.h"

MainWindow::MainWindow(QWidget *parent,  int argc, char *argv[]) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Logger::Init();

    connect(Logger::Object(), SIGNAL(addToLogger(QString)), this, SLOT(addToLogger(QString)));

    int port = 9572;
    if(argc > 1)
        port = QString(argv[1]).toInt();
    tcpServer = new TCPServer(port);
}

MainWindow::~MainWindow()
{
    delete tcpServer;
    delete ui;
}


void MainWindow::addToLogger(QString text)
{
    ui->txtLog->append(text);
    ui->txtLog->moveCursor(QTextCursor::End);
}

