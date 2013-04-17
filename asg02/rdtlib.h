// rdtlib.h

#ifndef RDTLIB_H
#define RDTLIB_H

#pragma once

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

#include "Timer.h"
#include "rdtpacket.h"

using namespace std;

typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int

// Time to wait between packet retransmissions (ms)
#define RETRANS_TIMEOUT 5000

// The number of retransmission attempts before generating a fatal error
#define RETRY_ATTEMPTS 5

/* Prototypes */
typedef struct pkt pkt_t;
pkt genPacket(char* chunk, int pSize, uint32_t seqno);
bool okToSend(uint32_t seqno, uint32_t lastACK);
pkt readPacket(int sockfd);
int readHeader(char *header);
uint32_t getLastACK(int sockfd);
uint16_t getLen();
char **splitData( const char * , int , int &);
bool timedOut( Timer );


int rdt_socket(int address_family, int type, int protocol);
int rdt_bind(int socket_descriptor, const struct sockaddr *local_address, socklen_t address_length);
int rdt_sendto(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *destination_address, int address_length);
int rdt_recv(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *from_address, int *address_length);
int close(int fildes);
void sendPacket( int sockfd, char* packet, int packetSize, struct sockaddr *dest_addr, int addr_len );


/* Functions */
int rdt_socket(int address_family, int type, int protocol)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	return sockfd;
}

int rdt_bind(int socket_descriptor, const struct sockaddr *local_address, socklen_t address_length)
{
	struct sockaddr local_addr = *local_address;
	int bindResult = bind(socket_descriptor,(struct sockaddr *)&local_addr,address_length);
	return bindResult;
}

int rdt_recv(int socket_descriptor, char *retBuffer, int buffer_length, int flags, struct sockaddr *from_address, int *address_length)
{
	socklen_t from_length = *address_length;
	char packetBuffer[HEADER_SIZE + DATA_SIZE];
	pkt *nullPacket = buildNullPacket( nullPacket );
	bool eof = false;
	size_t retBufSize = 0;


	while( !eof )
	{
			
		// read the packet from the socket
		if ( recvfrom( 	socket_descriptor, 
					packetBuffer, 
					buffer_length + HEADER_SIZE, 0,
					(struct sockaddr *)&from_address,
					&from_length ) < 0 )
		{
			perror("Packet recvfrom error");
		}

		/* create a packet out of the received data */
		pkt *recvPacket = buildPacket( recvPacket, packetBuffer );
	
		printf("Receiving packet: \n");
		printPacket( *recvPacket );

		/* check if we've reached the end, otherwise process the packet */
		if( *recvPacket == *nullPacket )
		{
			eof = true;
		} else
		{
			/* verify checksum the packet */	

			/* build the acknowledgement packet */
			pkt *ackPacket = buildAcknowledgementPacket( ackPacket, recvPacket->seqno + 1 );


			/* send the acknowledgement packet */
			if (sendto( socket_descriptor, 
						(const void *) &ackPacket, 
						HEADER_SIZE, 0, 
						(struct sockaddr *)&from_address, 
						from_length) < 0)
			{
				perror("acknowledgement sendto error");
			}

			/* append the data portion to the return buffer */
			retBuffer = appendData( recvPacket, retBuffer, retBufSize );
			printf("Sending acknowledgement packet: \n");
			printPacket( *ackPacket );
			
			retBufSize += recvPacket->len;
		}
		
	}	

	return 0;
}

int close(int fildes)
{
	close(fildes);
	return 0;
}

int rdt_sendto(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *destination_address, int address_length)
{
	uint32_t seqNumber = 0;
	int sockfd = socket_descriptor;
	string file(buffer);
	int lastPktSize, bufSize = buffer_length, curPktSize, curDataSize;
	Timer retryTimer;
	struct sockaddr dest_addr = *destination_address;

	// split data into chunks that are DATA_SIZE large
	char** chunks = splitData( buffer, bufSize, lastPktSize);

	bool eof = false;
	int fatal_error = 0;

	pkt curPacket;
	while (chunks != NULL && fatal_error < RETRY_ATTEMPTS )
	{
		/**
		 * Create the packet to be sent, if it's the last
		 * packet, set the proper size
		 **/
		curDataSize = chunks[seqNumber+1] == NULL ? lastPktSize : DATA_SIZE;
		curPacket = genPacket(chunks[seqNumber], curDataSize, seqNumber + 1);
		
		// Get the size of the entire packet
		curPktSize = curDataSize + HEADER_SIZE;

		printf("Sending Packet:\n");
		printPacket( curPacket );
		
		// send the generated packet!
		if (sendto(sockfd, (const void *) &curPacket, curPktSize, 0, (struct sockaddr *)&dest_addr, address_length) < 0)
		{
			perror("sento error");
		}

		/* Start the retry timer */
		retryTimer.Start();
		bool timeout = false;
		
		/**
		 *  While we haven't received an acknolwedgement, and the timeout
		 *  hasn't expired, wait. When one of these two conditions is met,
		 *  continue on.
		 **/
		while ( !okToSend( seqNumber + 1, getLastACK( sockfd )) && 
			(timeout = !timedOut( retryTimer ) ) );
		
		
		/**
		 * If a timeout did not occur, prepare to send the next packet
		 * otherwise, send the current packet over again.
		 **/
		if( timeout )
		{
			// increment the timeout counter
			fatal_error++;
		} 
		else 
		{
			// Don't need this chunk anymore
			delete chunks[seqNumber];
			
			// get ready to send the next chunk
			seqNumber++;

		}




	}

	// Clean up the array
	delete chunks;

	// if we got out of the loop because of retries, report the error
	if ( fatal_error == RETRY_ATTEMPTS )
	{
		perror("retransmission error, retry limit exceeded");
		return -1;
	}

	return 0;
}

/* Returns the status of the retry timer */
bool timedOut ( Timer timer )
{
	return timer.Elapsed() > RETRANS_TIMEOUT;
}

bool okToSend(uint32_t seqno, uint32_t lastACK)
{
	return seqno+1 == lastACK;
}

void sendPacket( int sockfd, char* packet, int packetSize, struct sockaddr *dest_addr, int addr_len )
{	
	if (sendto(sockfd, (const void *) &packet, packetSize, 0, dest_addr, addr_len < 0) ) 
	{
		perror("sendto error");
	}
}

pkt readPacket(int sockfd)
{
	socklen_t fromlen = sizeof( struct sockaddr_in );
	struct sockaddr_in from;
	int buflen = HEADER_SIZE;
	char *buf = new char[buflen];

	if (recvfrom(	sockfd,
					buf,
					buflen, 0,
					(struct sockaddr *) & from
					,&fromlen) < 0)
	{
		perror("recvfrom error");
	}

	pkt *recvPacket = buildPacket( recvPacket, buf );

	printPacket( *recvPacket );
	cout << "end readPacket" << endl;
	return *recvPacket;
}

uint32_t getLastACK(int sockfd)
{
	pkt lastPacket = readPacket(sockfd);	
	return lastPacket.ackno;
}

int readHeader(char* header)
{
	int result;
	pkt currPkt;
	bzero(&currPkt, sizeof(currPkt));
	memcpy(&currPkt, &header, sizeof(*header));
	cout << "Received seqno: " << currPkt.seqno << endl;
	cout << "Received length: " << currPkt.len << endl;
	cout << "Received checksum: " << currPkt.cksum << endl;
	return result;
}


pkt genPacket(char* chunk, int pSize, uint32_t seqno)
{
	pkt nextPacket;

	setPacketSize( &nextPacket, pSize );
	setSequenceNumber( &nextPacket, seqno );
	setAcknowledgementNumber( &nextPacket, 0x00 );
	setChecksum( &nextPacket );
	memcpy(nextPacket.data, chunk, pSize);
	return nextPacket;
}

uint16_t getLen()
{
	uint16_t len = DATA_SIZE + 12;
	return len;
}

/**
 *  Given a block of raw data and the amount of bytes it contains,
 *  break the data into chunks equal to the size of the data field
 *  in the packet. With the remaining data, stick the number into
 *  int lpSize.
 *  The chunk size is set by the DATA_SIZE define in rdtpacket.h
 *  
 *  Ex: dataSize is 1200
 *      return a 3 element array, two of them are 500, the last is 200
 *      lpsize will be 200
 **/
char **splitData( const char *data, int dataLength, int &lastPktSize)
{
	char** chunks;
	size_t numchunks, current = 0;
	lastPktSize = dataLength % DATA_SIZE;

	// the number of elements in the string array
	numchunks = dataLength / DATA_SIZE;

	// allocate enough slots to hold memory locations of the chunksz
	chunks = (char **) malloc (numchunks * sizeof( char* )) + 1;

	do
	{
		// Size of this chunk is DATA_SIZE, unless its the last element
		size_t thisChunkSize = (current == numchunks ) ? lastPktSize : DATA_SIZE;

		// Allocate memory for this chunk
		chunks[current] = (char *) malloc ( thisChunkSize );

		// Copy the data into the chunk
		memcpy( chunks[current], data + ( current * thisChunkSize ), thisChunkSize );
	
	} while( current++ < numchunks );

	// set the next chunk to NULL
	chunks[current] = NULL;

	return chunks;
}


#endif
