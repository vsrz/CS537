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
void TestRdtSender( int, char ** );
void TestRdtReceiver( int, char ** );
void TestComparison();
void TestPrintPacket();

int main( int argc, char **argv )
{
    //TestRdt( argc, argv );
    //TestPrintMemory();
    //TestChecksum();
    //TestPacketChunking();
    //TestTimerFiring();
	// TestPrintPacket();

   TestRdtSender( argc, argv );
   // TestRdtReceiver( argc, argv );
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

void TestComparison()
{
    char pd[1500] = "ASDF";
    char pc[1500] = "GHJK";
    pkt *p = createPacket( p, 0, 1, pd, 5 );
    pkt *q = createPacket( p, 0, 1, pc, 5 );

    // printMemory( &q->data, 5 );
    //pkt *q = buildNullPacket( q );

    //p->ackno = 0x02;

    //printf("%d\n", *p==*q);
    if (*p == *q) printf("Packets are Equal\n"); else printf("Not Equal\n");

    // pkt *n = buildNullPacket( p );
}

void TestPrintPacket()
{
    char pd[1500] = "ASDF";
    pkt *p = createPacket( p, 0, 1, pd, 5 );
    printPacket( *p );

}

void TestTimer()
{

    Timer t;
    int x = 1;
    t.Start();
    while ( x++ != 100 )
    {
        cout << "Current time is " << t.Elapsed() << endl;
        sleep(1);
        if (x % 6 == 0)
        {
            t.Stop();
            cout << "Stopped\n";
        }

        if (x % 10 == 0 )
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

    char **chunks;

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
