#ifndef _TCP_LOGGER_H_
#define _TCP_LOGGER_H_

#include "Message.h"
#include <semaphore.h> 

class TCPLogger
{
    private:
        static sem_t mutex; 
        static Message message;
    public:
        static void Start();
        static void Stop();
        static void Log(std::string text, int level = LEVEL_INFO);
};

#define LL TCPLogger

#endif
