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

#define BUFFER_SIZE 8912
#define DEFAULT_PORT 7777

using namespace std;
using namespace fileutils;

struct sockaddr_in g_dest_host;

void rdt_send(int fd, char *buf, size_t size)
{
	int res = 0;

	/* put this shit on the wire and pray */
	res = sendto(fd, buf, size + 1, 0, 
		(struct sockaddr*) &g_dest_host, sizeof g_dest_host);

	/* report any errors */
	if( res < 0 )
	{
		/* what's the problem folks */
		cout << strerror(errno) << "\n";
	}
}

int rdt_recv(int fd, char *buf, size_t size)
{
	string fileContents;
	// char *data;
	int dg_socket;
	struct sockaddr_in listen_host;

	/* setup listen addr struct */
	listen_host.sin_family = AF_INET;
	listen_host.sin_port = htons(DEFAULT_PORT);
	listen_host.sin_addr.s_addr = inet_addr("127.0.0.1");

	/* bind socket and wait for conn */
	dg_socket = socket(PF_INET, SOCK_STREAM, 0);
	bind( dg_socket, (struct sockaddr*) &listen_host, sizeof listen_host);

	return 0;
}

void sendFile(char *fn, char *bind_addr = NULL, int bind_port = DEFAULT_PORT)
{
	string fileContents;
	char *data;
	int dg_socket;
	struct sockaddr_in dest_host;

	/* read data file into string */
	readFile( fileContents, fn );

	/* build destination addr struct */
	if( bind_addr == NULL ) 
	{
		bind_addr = ( char* ) malloc(sizeof( char ) * 9);
		strcpy( bind_addr,"127.0.0.1" );
	}
	dest_host.sin_family = NULL;
	dest_host.sin_addr.s_addr = inet_addr(bind_addr);
	dest_host.sin_port = htons(bind_port);

	/* form the UDP socket */
	dg_socket = socket( AF_INET, SOCK_DGRAM, 0 );
	if( dg_socket < 0 ) 
	{
		cout << "Unable to create socket\n";
		exit(1);
	}

	/* assign to global for now */
	g_dest_host = dest_host;

	/* convert string to char and send it into rdt_send */
	data = (char *) malloc( sizeof( char )* fileContents.length() + 1);
	strcpy ( data, fileContents.c_str());
	
	rdt_send(dg_socket, data, fileContents.length());
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