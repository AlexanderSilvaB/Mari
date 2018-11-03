#ifndef _TCP_LOGGER_H_
#define _TCP_LOGGER_H_

#include "Message.h"
#include <semaphore.h> 
#include <sstream>

class TCPLogger;

class TCPLogStream : public std::ostringstream
{
    public:
	    TCPLogStream(TCPLogger& logger, int level);
	    TCPLogStream(const TCPLogStream& ls);
	    ~TCPLogStream();

private:
	TCPLogger& _logger;
	int _level;
};

class TCPLogger
{
    private:
        sem_t mutex; 
        Message message;
    public:
        TCPLogger();
        ~TCPLogger();
        void Log(std::string text, int level = LEVEL_INFO);

        TCPLogStream operator()();
	    TCPLogStream operator()(int level);
};

extern TCPLogger nlog;

#endif
