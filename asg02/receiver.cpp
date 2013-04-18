#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <vector>
#include <cctype>
#include <ctime>
#include <stdlib.h>
#include <vector>
#include <typeinfo>

#include "fileutils.h"
#include "Timer.h"
#include "memfn.h"
#include "rdtlib.h"
#include "rdtpacket.h"
#include "rdtip.h"

using namespace std;

#define MAXLINE 1024

void TestRdtReceiver( int, char ** );

int main( int argc, char **argv )
{

    TestRdtReceiver( argc, argv );
    return 0;
}

void TestRdtReceiver( int argc, char **argv )
{
    int recvsock, length, n;
    int fromlen;
    struct sockaddr_in server;
    struct sockaddr_in from;    
    int bufferSize = 500;
    char buffer[bufferSize];
    string data = "";

    if (argc != 3)
    {
        cout << "Usage: rdt_receiver <address> <port> \n";
        return;
    }

    recvsock = bindSocket();
    length = sizeof(server);
    server = setIPAddress( argv[1], atoi( argv[2] ));

    from.sin_family = AF_INET;
    fromlen = sizeof(struct sockaddr_in);

    if (rdt_bind( recvsock, (struct sockaddr *) &server, length ) < 0)
    {
        cout << "Error binding socket\n";
        return;
    }

    int i=0;
    /* while the eof has not been reached */
    while ( rdt_recv( recvsock, buffer, bufferSize, 0, (struct sockaddr *) &from, &fromlen ) > 0 )
    {    	
    	data += buffer;
        bzero(buffer, bufferSize);
    } 
   
    cout << data << endl; 
    // Grab the last piece of the buffer
    fileutils::writeFileToDisk(data, "output.txt");
    
}

