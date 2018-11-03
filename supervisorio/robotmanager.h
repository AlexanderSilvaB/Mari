#ifndef ROBOTMANAGER_H
#define ROBOTMANAGER_H

#include <QList>
#include <QTreeWidget>
#include <QThread>
#include <QHostInfo>
#include "robot.h"

class RobotManagerWorker : public QThread
{
    Q_OBJECT

    private:
        bool keepRunning, search;
        QList<Robot> *robots;

        QString manualRobot;

        void broadcast();
        void findAddress();
        void checkRobot(QString ip, bool manual);
        void createRobot(QString hostname, QString ip, bool manual);
        int findRobotIndex(QString ip);
        void clearRobots();
    public:
        RobotManagerWorker(QList<Robot> *robots);

        void run() override;

        bool isSearching();
        void startSearch();
        void stopSearch();
        void stopRun();

        bool addRobot(QString ip);
    signals:
        void resultReady();
};

class RobotManager : public QObject
{
    Q_OBJECT
    private:
        QList<Robot> robots;
        RobotManagerWorker *worker;
        bool running;
        QTreeWidget *listWidget;
    public:
        RobotManager();
        ~RobotManager();

        bool isRunning();

        void setWidget(QTreeWidget *listWidget);
        void startSearch();
        void stopSearch();

        bool addRobot(QString ip);

    public slots:
        void handleResults();
};

#endif // ROBOTMANAGER_H
