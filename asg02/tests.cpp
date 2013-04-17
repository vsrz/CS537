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

void TestTimer()
{

	Timer t;
	int x = 1;
	t.Start();
	while( x++ != 100 ) 
	{	
		cout << "Current time is " << t.Elapsed() << endl;
		sleep(1);
		if(x % 6 == 0) 
		{
			t.Stop();
			cout << "Stopped\n";
		}

		if(x % 10 == 0 ) 
		{
			t.Start();
			cout << "Restart\n";
		}


	}
	t.Stop();

}

void TestRdt( int argc, char** argv )
{
	myrdtlib rdt;
	struct  sockaddr_in servaddr;
	int     sockfd;
	int     servlen;
	int     ntimes;
	int     n;
	char    buf[1];
	char    ptime[MAXLINE];
	char data[256];
	string sdata;

	if (argc != 4) 
	{
		printf("Usage: udp_client <address> <port> <ntimes>\n");
		exit(-1);
	}

	// create rdt socket
	sockfd = rdt.rdt_socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) 
	{
		perror("socket error");
		exit(-1);
	}

	// setup ip address
	servaddr = setIPAddress( argv[1], atoi( argv[2] ) );
	servlen = sizeof(servaddr);

	ntimes = atoi(argv[3]);

	bool exit = false;
	while (!exit) 
	{
	        cout << "\nEnter data: ";
	        cin >> sdata;
		if (sdata == "exit")
		{
			exit = true;
		}

		char *cdata = new char[sdata.size()];
		cdata[sdata.size()] = 0;
		memcpy(cdata, sdata.c_str(), sdata.size());
		if (rdt.rdt_sendto(sockfd, (char*)cdata, sdata.size(), 0, (struct sockaddr *)&servaddr, servlen) < 0) 
		{
			perror("sendto error");
			return;
		}

		n = rdt.rdt_recv(sockfd, ptime, MAXLINE, 0, (struct sockaddr *)&servaddr, (int*)&servlen);
		fputs(ptime, stdout);
		delete cdata;
		sleep(1);
	}

}


int main( int argc, char** argv )
{
	//TestRdt( argc, argv );	
	return 0;
}

