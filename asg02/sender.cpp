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

void TestRdtSender( int, char ** );


int main( int argc, char **argv )
{
    
    TestRdtSender( argc, argv );
    return 0;
}


void TestRdtSender( int argc, char **argv )
{
    struct  sockaddr_in servaddr;
    int     sockfd;
    int     servlen;

    if (argc != 4)
    {
        printf("Usage: rdt_sender <address> <port> <filename>\n");
        exit(-1);
    }

    // Bind the socket
    sockfd = bindSocket();

    // setup the remote ip addr
    servaddr = setIPAddress( argv[1], atoi( argv[2] ));
    servlen = sizeof(servaddr);

    /* load the sample data file for sending */
    string filedata;
    fileutils::readFile( filedata, argv[3] );

    // send the data
    if (rdt_sendto(sockfd, (char *)filedata.c_str(), filedata.size(), 0, (struct sockaddr *)&servaddr, servlen) < 0)
    {
        perror("sendto error");
        return;
    }      

}
