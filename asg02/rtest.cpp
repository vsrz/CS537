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
//#include "fileutils.h"
#include "Timer.h"
#include "myrdtlib.h"


using namespace std;

#define MAXLINE 1024 
#define BUF_SIZE 1512


void rTestRdt( int argc, char** argv )
{
	myrdtlib rdt;
	int sock, length, n;
	int fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char buf[BUF_SIZE];

	if (argc != 3)
	{
		cout << "Usage: udp_server <address> <port> \n";
		return;
	}

	sock=rdt.rdt_socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		cout << "error opening socket\n";
		return;
	}
	length = sizeof(server);
	bzero(&server,length);
	server.sin_family=AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port=htons(atoi(argv[2]));
	
	from.sin_family=AF_INET;
	
	if (rdt.rdt_bind(sock,(struct sockaddr *)&server,length)<0)
	{
		cout << "Error binding\n";
		return;
	}
	fromlen = sizeof(struct sockaddr_in);
	while (1) 
	{
		n = rdt.rdt_recv(sock,buf,BUF_SIZE,0,(struct sockaddr *)&from,&fromlen);
		if (n < 0)
		{
			cout << "recvfrom error\n";
			return;
		}
		string data(buf);
		cout << "Received a datagram, size: " << data.size() << ", contents: " << buf << endl;
//		write(1,"Received a datagram: ",21);
//		write(1,buf,n);
		n = rdt.rdt_sendto(sock,"ACK",3, 0,(struct sockaddr *)&from,fromlen);
		if (n  < 0)
		{
			cout << "Error sending\n";
			return;
		}
	}

}


int main( int argc, char** argv )
{
	rTestRdt( argc, argv );	
	return 0;
}