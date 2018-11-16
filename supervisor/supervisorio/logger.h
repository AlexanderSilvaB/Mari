#ifndef LOGGER_H
#define LOGGER_H

#include <QTextEdit>
#include "message.h"

class Logger : public QObject
{
    Q_OBJECT

    private:
        static Logger *logger;
        static void output(QString &text);
        void callEmit(QString &text);
    public:
        static void Init();
        static Logger *Object();
        static void log(Message *message);
        static void log(QString text, int level = LEVEL_INFO);

    signals:
        void addToLogger(QString text);
};

#endif // LOGGER_H
