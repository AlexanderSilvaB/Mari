#include "logger.h"
#include <QDebug>

Logger* Logger::logger = NULL;

void Logger::Init()
{
    logger = new Logger;
}

void Logger::log(Message *message)
{
    QString base = "";
    switch (message->getLevel())
    {
        case LEVEL_INFO:
            base = "<font color='#219e27'>I [%1] <b>%2</b></font>";
            break;
        case LEVEL_DEBUG:
            base = "<font color='#162a99'>D [%1] <b>%2</b></font>";
            break;
        case LEVEL_WARNING:
            base = "<font color='#e0ac00'>W [%1] <b>%2</b></font>";
            break;
        case LEVEL_ERROR:
            base = "<font color='#c90000'>E [%1] <b>%2</b></font>";
            break;
        default:
            base = "<font color='#219e27'>I [%1] <b>%2</b></font>";
            break;
    }
    QString dateTime = message->getDateTime().toString("hh:mm:ss dd/MM/yyyy");
    QString text = base.arg(dateTime, message->toString());
    output(text);
}

void Logger::log(QString text, int level)
{
    Message message(text);
    message.setLevel(level);
    log(&message);
}

void Logger::output(QString &text)
{
    logger->callEmit(text);
}

Logger *Logger::Object()
{
    return logger;
}

void Logger::callEmit(QString &text)
{
    emit addToLogger(text);
}
