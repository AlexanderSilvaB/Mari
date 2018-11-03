#include "TCPLogger.h"
#include <iostream>
#include "Modules/Network/NetworkModule.h"

using namespace std;

TCPLogger nlog;

TCPLogger::TCPLogger()
{
    sem_init(&mutex, 0, 1); 
}

TCPLogger::~TCPLogger()
{
    sem_destroy(&mutex); 
}

TCPLogStream TCPLogger::operator()()
{
	return TCPLogStream(*this, LEVEL_DEBUG);
}

TCPLogStream TCPLogger::operator()(int level)
{
	return TCPLogStream(*this, level);
}

void TCPLogger::Log(string text, int level)
{
    sem_wait(&mutex);
    message.setLevel(level);
    message.setMessage(text);
    NetworkModule::SendMessage(&message);
    sem_post(&mutex);

    switch(level)
    {
        case LEVEL_INFO:
            cout << "INFO: " << text << endl;
            break;
        case LEVEL_DEBUG:
            cout << "DEBUG: " << text << endl;
            break; 
        case LEVEL_WARNING:
            cout << "WARNING: " << text << endl;
            break; 
        case LEVEL_ERROR:
            cout << "ERROR: " << text << endl;
            break; 
        default:   
            break;
    }
}

TCPLogStream::TCPLogStream(TCPLogger& logger, int level) :
    _logger(logger), _level(level)
{
}

TCPLogStream::TCPLogStream(const TCPLogStream& ls) :
    _logger(ls._logger), _level(ls._level)
{
}

TCPLogStream::~TCPLogStream()
{
	_logger.Log(str(), _level);
}