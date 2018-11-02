#include "TCPLogger.h"
#include "Modules/Network/NetworkModule.h"

using namespace std;

Message TCPLogger::message;
sem_t TCPLogger::mutex; 

void TCPLogger::Start()
{
    sem_init(&mutex, 0, 1); 
}

void TCPLogger::Stop()
{
    sem_destroy(&mutex); 
}

void TCPLogger::Log(string text, int level)
{
    sem_wait(&mutex);
    message.setLevel(level);
    message.setMessage(text);
    NetworkModule::SendMessage(&message);
    sem_post(&mutex);
}