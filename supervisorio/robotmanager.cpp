#include "robotmanager.h"
#include <QDebug>
#include <QNetworkInterface>
#include <QProcess>
#include <QHostInfo>
#include "logger.h"

RobotManagerWorker::RobotManagerWorker(QList<Robot> *robots)
{
    this->robots = robots;
    this->keepRunning = true;
    this->search = false;
}

void RobotManagerWorker::run()
{
    while(keepRunning)
    {
        if(search)
        {
            findAddress();
            if(!manualRobot.isNull() && !manualRobot.isEmpty())
            {
                checkRobot(manualRobot, true);
                manualRobot = "";
            }
            clearRobots();
            emit resultReady();
            QThread::sleep(30);
        }
        QThread::sleep(2);
    }
}

void RobotManagerWorker::broadcast()
{
    QProcess process;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    for(int nIter=0; nIter<list.count(); nIter++)
    {
        if(!list[nIter].isLoopback())
            if (list[nIter].protocol() == QAbstractSocket::IPv4Protocol )
            {
                QString address = list[nIter].toString();
                QString base = address.left(address.lastIndexOf('.'));
                process.start("ping", QStringList() << "-b" << "-c" << "1" << (base+".255"));
                process.waitForFinished();
            }
    }
}

void RobotManagerWorker::findAddress()
{
    QProcess process;
#ifdef Q_OS_WIN32
    broadcast();
    process.start("arp", QStringList() << "-a");
#else
    process.start("avahi-browse", QStringList() << "-at");
#endif
    process.waitForFinished();
    QString output(process.readAllStandardOutput());
#ifdef Q_OS_WIN32
    QStringList lines = output.split("\r\n", QString::SplitBehavior::SkipEmptyParts);
#else
    QStringList lines = output.split("\n", QString::SplitBehavior::SkipEmptyParts);
#endif
    for (int i = 0; i < lines.size(); i++)
    {
        QString line = lines[i].trimmed();
        #ifdef Q_OS_WIN32
        if(line.length() > 0 && line.at(0).isDigit())
        {
            QString ip = line.left(line.indexOf(' '));
            checkRobot(ip);
        }
        #else
        QStringList parts = line.split(" ", QString::SplitBehavior::SkipEmptyParts);
        if(parts.length() > 5)
        {
            if(parts[2] == "IPv4" && !parts[3].contains("@") && parts[4] == "_naoqi._tcp")
            {
                checkRobot(parts[3]+".local", false);
            }
        }
        #endif
    }
}

void RobotManagerWorker::checkRobot(QString ip, bool manual)
{
    QHostInfo info = QHostInfo::fromName(ip);
    if(info.addresses().size() > 0)
        createRobot(info.hostName(), info.addresses().first().toString(), manual);
}

void RobotManagerWorker::createRobot(QString hostname, QString ip, bool manual)
{
    int index = findRobotIndex(ip);
    if(index == -1)
    {
        Robot robot;
        robot.ip = ip;
        robot.hostName = hostname;
        robot.updated = true;
        robot.manual = manual;
        robots->append(robot);
    }
    else
    {
        (*robots)[index].hostName = hostname;
        (*robots)[index].updated = true;
    }
}

int RobotManagerWorker::findRobotIndex(QString ip)
{
    for(int i = 0; i < robots->size(); i++)
    {
        if((*robots)[i].ip == ip)
            return i;
    }
    return -1;
}

void RobotManagerWorker::clearRobots()
{
    QList<Robot>::iterator it = robots->begin();
    while (it != robots->end())
    {
        if(!it->updated)
            it = robots->erase(it);
        else
            it++;
    }
}

bool RobotManagerWorker::isSearching()
{
    return search;
}

void RobotManagerWorker::startSearch()
{
    search = true;
}

void RobotManagerWorker::stopSearch()
{
    search = false;
}

void RobotManagerWorker::stopRun()
{
    keepRunning = false;
}

bool RobotManagerWorker::addRobot(QString ip)
{
    manualRobot = ip;
    return true;
}

RobotManager::RobotManager()
{
    worker = new RobotManagerWorker(&robots);
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &RobotManagerWorker::resultReady, this, &RobotManager::handleResults);
    worker->start();
    listWidget = nullptr;
}

RobotManager::~RobotManager()
{
    worker->stopSearch();
    worker->stopRun();
    worker->quit();
    worker->wait();
}

bool RobotManager::isRunning()
{
    return worker->isSearching();
}

void RobotManager::setWidget(QTreeWidget *listWidget)
{
    this->listWidget = listWidget;
}

void RobotManager::startSearch()
{
    if(!isRunning())
    {
        worker->startSearch();
    }
}

void RobotManager::stopSearch()
{
    if(isRunning())
    {
        worker->stopSearch();
    }
}

void RobotManager::handleResults()
{
    if(listWidget != nullptr)
    {
        for(int i = 0; i < robots.size(); i++)
        {
            QList<QTreeWidgetItem*> items = listWidget->findItems(robots[i].ip, Qt::MatchFlag::MatchExactly, 1);
            if(items.size() > 0)
            {
                items[0]->setText(0, robots[i].hostName);
            }
            else
            {
                QTreeWidgetItem *robot = new QTreeWidgetItem();
                robot->setText(0, robots[i].hostName);
                robot->setText(1, robots[i].ip);
                listWidget->addTopLevelItem(robot);
                Logger::log("Robot added "+robots[i].hostName);
            }
        }
        QList<QTreeWidgetItem*> items = listWidget->findItems("*", Qt::MatchFlag::MatchWildcard, 0);
        QList<QTreeWidgetItem*>::iterator it = items.begin();
        bool remove = false;
        while (it != items.end())
        {
            remove = true;
            for(int i = 0; i < robots.size(); i++)
            {
                if((*it)->text(1) == robots[i].ip)
                {
                    remove = false;
                    break;
                }
            }
            if(remove)
                it = items.erase(it);
            else
                it++;
        }
    }
}

bool RobotManager::addRobot(QString ip)
{
    return worker->addRobot(ip);
}

