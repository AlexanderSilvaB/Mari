#include "Message.h"
#include <cstring>

using namespace std;


int Message::ID = 0;

void Message::initId()
{
    id = ID++;
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
    initMessage();
    initLevel();
}

Message::Message(long id)
{
    setType(TYPE_BASIC);
    this->id = id;
    initLevel();
}

Message::Message(string &message)
{
    setType(TYPE_BASIC);
    initId();
    this->message = message;
    initLevel();
}

Message::Message(long id, string &message)
{
    setType(TYPE_BASIC);
    this->id = id;
    this->message = message;
    initLevel();
}

Message::Message(const Message &message)
{
    initId();
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

void Message::setMessage(const string &message)
{
    this->message = message;
}

string &Message::getMessage()
{
    return message;
}

string Message::toString()
{
    return message;
}

int Message::decode(vector<char> &data)
{
    int sz = 0;
    if(data.size() < 4)
        return sz;
    char *ptr = data.data();
    if(!(ptr[0] == 'R' && ptr[1] == 'I' && ptr[2] == 'N' && ptr[3] == 'O'))
        return sz;
    sz += 4;
    
    type = ((int*)ptr + sz)[0];
    sz += sizeof (type);
    level = ((int*)ptr + sz)[0];
    sz += sizeof (level);
    int msgLen = ((int*)ptr + sz)[0];
    sz += sizeof (msgLen);
    char *str = new char[msgLen+1];
    memcpy(str, ptr + sz, msgLen);
    str[msgLen] = '\0';
    sz += msgLen;
    message = string(str);
    return sz;
}

int Message::encode(vector<char> &data)
{
    char buff[message.size() + 128];
    int sz = 0;

    const char *init = "RINO";
    memcpy(buff + sz, init, 4);
    sz += 4;
    memcpy(buff + sz, (char*)(&type), sizeof(type));
    sz += 4;
    memcpy(buff + sz, (char*)(&level), sizeof(level));
    sz += 4;
    int len = message.length();
    memcpy(buff + sz, (char*)(&len), sizeof(len));
    sz += 4;
    memcpy(buff + sz, message.c_str(), len);
    for(int i = 0; i < sz; i++)
        data.push_back(buff[i]);
    return data.size();
}
