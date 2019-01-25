# TcpUdpSocket
C++ Class for UDP/TCP communications for Windows/Unix

This project is based on DatagramSocket from Glenn Butcher
https://github.com/butcherg/DatagramSocket


## Usage
```C++
#include "TcpUdpSocket.h"
#include "Thread.h"  //another project
#include <iostream>
#include <string>

using namespace std;

class MyThread: public Thread
{
    private:
        TcpUdpSocket *s;

    public:
	MyThread(TcpUdpSocket *sock)
	{
	    s = sock;
		Thread::CreateNewThread(this);
	}

	void Run(void*)
	{
	    char msg[4000];
		while (1) {
		    s->receive(msg, 4000);
		    cout << msg << endl;
		}
	}

};


int main(int argc, char *argv[])
{
    TcpUdpSocket *s = new TcpUdpSocket(5000, "255.255.255.255", TRUE, TRUE, TRUE);
	MyThread *t = new MyThread(s);

	string msg = "";

	while (1)  {
	    getline(cin, msg);
	    s->send(msg.c_str(), msg.length());
	}
	return 0;
}
```

The above example uses the Thread class from Glenn Butcher to implement a simple chat client. 
https://github.com/butcherg/Thread

In Windows you will need to add wsock32 to 
the linked libraries. Note that CMakeLists.txt is already doing that.