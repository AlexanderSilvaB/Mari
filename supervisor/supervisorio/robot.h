#ifndef ROBOT_H
#define ROBOT_H

#include <QString>

typedef struct
{
    QString ip;
    QString hostName;
    bool naoQiRunning;
    int batteryLevel;
    int maxTemperature;
    int wifiSignal;
    bool wifiConnected;
    bool updated;
    bool manual;
}Robot;

#endif // ROBOT_H
