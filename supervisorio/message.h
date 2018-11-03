#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDateTime>
#include <QDataStream>

#define TYPE(x) ((int)x)

#define LEVEL_INFO 1
#define LEVEL_DEBUG 2
#define LEVEL_WARNING 3
#define LEVEL_ERROR 4

#define TYPE_BASIC TYPE('B'+'A'+'S'+'I'+'C')

class Message
{
    private:
        long id;
        int level, type;
        QDateTime dateTime;
        QString message;
        void initId();
        void initDateTime();
        void initMessage();
        void initLevel();

    protected:
        void setType(int type);

    public:
        Message();
        Message(long id);
        Message(QString &message);
        Message(long id, QString &message);
        Message(QDateTime &dateTime, QString &message);
        Message(long id, QDateTime &dateTime, QString &message);
        Message(const Message &message);
        virtual ~Message();
        int getType();
        int getLevel();
        void setLevel(int level);
        void setDateTime(const QDateTime& dateTime);
        QDateTime &getDateTime();
        void setMessage(const QString &message);
        QString &getMessage();

        virtual QString toString();
        virtual int decode(QByteArray &data);
        virtual int encode(QByteArray &data);

};

#endif // MESSAGE_H
