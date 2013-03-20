#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <fstream>
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
#include <stdlib.h>
#include <ctime>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "fileutils.h"
#include "dateutils.h"

using namespace std;
using namespace fileutils;

void rdt_send(int fd, char *buf, size_t size)
{


}

int rdt_recv(int fd, char *buf, size_t size)
{


}

void sendFile(char *fn)
{
	string fileContents;
	readFile( fileContents, fn );
	cout << fileContents << endl;
	
}

void recvFile(char *file)
{



}

void printHelp(char* arg)
{
	cout << "Usage: " << arg << " <send|recv> [filename] [bind ip] [port]\n";
	cout << "CS537 S13 / UDP Communicator. Sends and receives data over UDP port specified\n";
	cout << "  send|recv    Choose send mode or receive mode\n";
	cout << "  filename     Writes or reads the file containing data received to disk. Default: stdout\n";
	cout << "  bind ip      IP address to bind to. Default: 127.0.0.1.\n";
	cout << "  port         UDP Port to communicate on. Default: 7777\n";
	exit(1);
}

int main(int argc, char** argv)
{
	if( argc < 2)
	{
		printHelp(argv[0]);
	}

	if( strcmp(argv[1],"send") == 0 )
	{
		sendFile(argv[2]);

	} else if( strcmp(argv[1],"recv") == 0 )
	{
		recvFile(argv[2]);
	} else printHelp(argv[0]);

	return 0;
}