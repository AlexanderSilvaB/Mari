#include <QDataStream>
#include "message.h"

void Message::initId()
{
    QDateTime UTC(QDateTime::currentDateTimeUtc());
    id = UTC.toMSecsSinceEpoch();
}

void Message::initDateTime()
{
    QDateTime UTC(QDateTime::currentDateTimeUtc());
    dateTime = UTC.toLocalTime();
}

void Message::initMessage()
{
    message = "";
}

void Message::initLevel()
{
    level = LEVEL_INFO;
}

Message::Message()
{
    setType(TYPE_BASIC);
    initId();
    initDateTime();
    initMessage();
    initLevel();
}

Message::Message(long id)
{
    setType(TYPE_BASIC);
    this->id = id;
    initLevel();
}

Message::Message(QString &message)
{
    setType(TYPE_BASIC);
    initId();
    initDateTime();
    this->message = message;
    initLevel();
}

Message::Message(long id, QString &message)
{
    setType(TYPE_BASIC);
    this->id = id;
    this->message = message;
    initDateTime();
    initLevel();
}

Message::Message(QDateTime &dateTime, QString &message)
{
    setType(TYPE_BASIC);
    initId();
    this->dateTime = dateTime;
    this->message = message;
    initLevel();
}

Message::Message(long id, QDateTime &dateTime, QString &message)
{
    setType(TYPE_BASIC);
    this->id = id;
    this->dateTime = dateTime;
    this->message = message;
    initLevel();
}

Message::Message(const Message &message)
{
    initId();
    setDateTime(message.dateTime);
    setLevel(message.level);
    setMessage(message.message);
    setType(message.type);
}

Message::~Message()
{

}

void Message::setType(int type)
{
    this->type = type;
}

int Message::getType()
{
    return type;
}

void Message::setLevel(int level)
{
    this->level = level;
}

int Message::getLevel()
{
    return level;
}

void Message::setDateTime(const QDateTime& dateTime)
{
    this->dateTime = dateTime;
}

QDateTime &Message::getDateTime()
{
    return dateTime;
}

void Message::setMessage(const QString &message)
{
    this->message = message;
}

QString &Message::getMessage()
{
    return message;
}

QString Message::toString()
{
    return message;
}

int Message::decode(QByteArray &data)
{
    int sz = 0;
    if(data.size() < 4)
        return sz;
    char *ptr = data.data();
    if(!(ptr[0] == 'R' && ptr[1] == 'I' && ptr[2] == 'N' && ptr[3] == 'O'))
        return sz;
    sz += 4;
    QDataStream stream(data);
    stream.skipRawData(4);
    stream >> type;
    sz += sizeof (type);
    stream >> level;
    sz += sizeof (level);
    int msgLen;
    stream >> msgLen;
    sz += sizeof (msgLen);
    char *str = new char[msgLen+1];
    stream.readRawData(str, msgLen);
    sz += msgLen;
    str[msgLen] = '\0';
    message = QString(str);
    return sz;
}

int Message::encode(QByteArray &data)
{
    QDataStream stream(&data, QIODevice::ReadWrite);
    const char *init = "RINO";
    stream.writeRawData(init, 4);
    stream << type;
    stream << level;
    stream << message.length();
    stream.writeRawData(message.toStdString().c_str(), message.length());
    return data.size();
}
