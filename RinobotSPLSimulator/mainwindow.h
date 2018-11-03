#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tcpserver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr, int argc = 0, char *argv[] = NULL);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TCPServer *tcpServer;

private slots:
    void    addToLogger(QString text);
};

#endif // MAINWINDOW_H
