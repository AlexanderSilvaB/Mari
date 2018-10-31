#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <string>
#include <vector>

#define TYPE(x) ((int)x)

#define LEVEL_INFO 1
#define LEVEL_DEBUG 2
#define LEVEL_WARNING 3
#define LEVEL_ERROR 4

#define TYPE_BASIC TYPE('B'+'A'+'S'+'I'+'C')

class Message
{
    private:
        static int ID;
        long id;
        int level, type;
        std::string message;
        void initId();
        void initMessage();
        void initLevel();

    protected:
        void setType(int type);

    public:
        Message();
        Message(long id);
        Message(std::string &message);
        Message(long id, std::string &message);
        Message(const Message &message);
        virtual ~Message();
        int getType();
        int getLevel();
        void setLevel(int level);
        void setMessage(const std::string &message);
        std::string &getMessage();

        virtual std::string toString();
        virtual int decode(std::vector<char> &data);
        virtual int encode(std::vector<char> &data);
};

#endif
