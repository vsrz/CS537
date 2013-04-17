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

void TestTimer();
void TestPrintMemory();
void TestChecksum();
void TestPacketChunking();
void TestTimerFiring();
void TestRdtSender( int, char** );
void TestRdtReceiver( int, char** );

int main( int argc, char** argv )
{
	//TestRdt( argc, argv );	
	//TestPrintMemory();
	//TestChecksum();
	//TestPacketChunking();
	//TestTimerFiring();
	TestRdtSender( argc, argv );
	return 0;
}

void TestRdtReceiver( int argc, char** argv )
{
	int recvsock, length, n;
	int fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char buffer[DATA_SIZE];

	if (argc != 3)
	{
		cout << "Usage: rdt_receiver <address> <port> \n";
		return;
	}

	recvsock = bindSocket();
	
	length = sizeof(server);
	server = setIPAddress( argv[1], atoi( argv[2] ));

	from.sin_family=AF_INET;
	fromlen = sizeof(struct sockaddr_in);
		
	if (rdt_bind( recvsock, (struct sockaddr *) &server, length ) < 0)
	{
		cout << "Error binding socket\n";
		return;
	}

	// 
	string writebuf;

	/* wait for a connection */
	while (1) 
	{
		pkt *recvPacket = new struct pkt;
		pkt *ackPacket = new struct pkt;

		n = rdt_recv( recvsock, buffer, DATA_SIZE, 0, (struct sockaddr *) &from, &fromlen );
		if (n < 0)
		{
			cout << "recvfrom error\n";
			return;
		}

		/* create a packet out of the received data */
		recvPacket = buildPacket( recvPacket, buffer );

		/* verify checksum the packet */

		/* build the acknowledgement packet */
		ackPacket = buildAcknowledgementPacket( ackPacket, recvPacket->seqno + 1 );

		/* send the acknowledgement */
		n = rdt_sendto( recvsock, (char *)ackPacket, HEADER_SIZE, 0,(struct sockaddr *) &from, fromlen);
		if (n < 0)
		{
			cout << "Error sending acknowledgement\n";
			return;
		}
	}

}

void TestRdtSender( int argc, char** argv )
{
	struct  sockaddr_in servaddr;
	int     sockfd;
	int     servlen;

	if (argc != 3) 
	{
		printf("Usage: rdt_sender <address> <port>\n");
		exit(-1);
	}

	// Bind the socket
	sockfd = bindSocket(); 

	// setup the remote ip addr
	servaddr = setIPAddress( argv[1], atoi( argv[2] ));
	servlen = sizeof(servaddr);

	/* load the sample data file for sending */
	string filedata;
	fileutils::readFile( filedata, "sample.txt" );

	// allocate a chunk of memory to put the data blob into
	char *cdata = new char[filedata.size()];

	// copy the memory into the newly created block
	memcpy(cdata, filedata.c_str(), filedata.size());

	// attempt to send the data
	if (rdt_sendto(sockfd, (char*)cdata, filedata.size(), 0, (struct sockaddr *)&servaddr, servlen) < 0) 
	{
		perror("sendto error");
		return;
	}

	delete[] cdata;
	sleep(1);

}


void TestTimerFiring() 
{
	Timer t;
	bool b;
	t.Start();

	// should quit immediately, since b = time_elapsed > 5000 is false
	while ( b = t.Elapsed() > 5000 )
	{
		sleep(1);
		printf("firing.\n");
	}

}

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

void TestPacketChunking( void )
{
	char *c = new char[45];
	strcpy(c, "The quick brown fox jumps over the lazy dog.");
	int lastPktSize = 0;
	int bufferLength = 46; // include null character

	char** chunks;

	chunks = splitData( c, bufferLength, lastPktSize );

	printf("%s\n%d\n", chunks[0], lastPktSize);
	delete[] c;

}
void TestChecksum( void )
{
	pkt p;
	char data[] = "The quick brown fox jumps over the lazy dog.\n";
	p.len = 45;
	p.ackno = 0;
	p.seqno = 0;
	p.cksum = 0;
	memcpy( &p.data, &data, 1500);

	setChecksum( &p );

	printMemory( &p.len, sizeof(uint16_t) );

}

void TestPrintMemory ( void )
{
	char c[] = "The quick brown fox jumps over the lazy dog.\n";
	unsigned short int a = 32000;
	printMemory( &c, 46 );	
	printMemory( &a, sizeof ( uint16_t ));

}
