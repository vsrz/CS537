#ifndef MYRDTLIB_CPP
#define MYRDTLIB_CPP

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
#include "myrdtlib.h"

using namespace std;
/*
typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int

//bool sqn = false;

#define PAYLOAD_SIZE 50
*/


int myrdtlib::rdt_socket(int address_family, int type, int protocol)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	return sockfd;
}

int myrdtlib::rdt_bind(int socket_descriptor, const struct sockaddr *local_address, socklen_t address_length)
{
	int bindResult = bind(socket_descriptor,(struct sockaddr *)&local_address,address_length);
	return bindResult;
}

int myrdtlib::rdt_recv(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *from_address, int *address_length)
{
	socklen_t from_length = *address_length;
	int n = recvfrom(socket_descriptor,buffer,buffer_length,0,(struct sockaddr *)&from_address,&from_length);
	return n;
}

int myrdtlib::close(int fildes)
{
	close(fildes);
	return 0;
}

int myrdtlib::rdt_sendto(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *destination_address, int address_length)
{
	uint32_t sqn = 0;
	int sockfd = socket_descriptor;
	string file(buffer);
	int bufLen = buffer_length;
	int lpSize;
	//struct sockadder_in = destination_address;
	
	char** chunks = myrdtlib::PacketChunking(file, lpSize);
	bool eof = false;
	int i = 0;
	pkt nextPacket;
	while (!eof)
	{
		if (chunks[i+1] != NULL)
		{
			nextPacket = genPacket(chunks[i], 1500, sqn);
		}
		else
		{
			nextPacket = genPacket(chunks[i], (buffer_length % 1500), sqn);
			eof = true;
		}
		i++;
		
		bool ready = false;
		while (!ready)
		{
			//this is what needs to be interrupted after a timeout
			ready = okToSend(nextPacket.seqno, getLastACK(sockfd));
		}
		
		//if interrupted, send previous pkt
		char* cPacket = new char[nextPacket.len];
		memcpy(&nextPacket, &cPacket, nextPacket.len);
		if (sendto(sockfd, (const void *)cPacket, sizeof(cPacket), 0, (struct sockaddr *)&destination_address, address_length) < 0) 
		{
			perror("sendto error");
			return -1;
		}
		delete cPacket;
	}
	return 0;
}

bool myrdtlib::okToSend(uint32_t seqno, uint32_t lastACK)
{
	if (seqno == 1 || seqno == lastACK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

myrdtlib::pkt myrdtlib::readPacket(int sockfd)
{
	pkt lastPacket;
	socklen_t fromlen;
	struct sockaddr_in from;
	char buf[8];
	memset(buf, 0, 8);
	int n = recvfrom(sockfd,buf,8,0,(struct sockaddr *)&from,&fromlen);
	if (n < 0)
	{
		cout << "recvfrom error\n";
	}
	memcpy(&buf, &lastPacket, 8);
	return lastPacket;
}

uint32_t myrdtlib::getLastACK(int sockfd)
{
	pkt lastPacket = readPacket(sockfd);
	uint32_t lastACK;
	char buf[8];
	memcpy(&lastPacket, &buf, 8); //copy the whole pkt into the buf
	memcpy(&buf[4], &lastACK, 4); //copy just the ACK field into the uint32_t
	return lastACK;
}

char** myrdtlib::PacketChunking(string file, int &lpSize)
{
	char** chunks;
	char* pkt;

	lpSize = file.size() % 1512;

	//readFile( file, "sample.txt" );
	chunks = chunkData( file, PAYLOAD_SIZE );

	//pkt = createPacket( chunks[2], PAYLOAD_SIZE, "____________", 12 );
	//std::cout << pkt << endl;
	
	return chunks;
}



myrdtlib::pkt myrdtlib::genPacket(char* chunk, int pSize, uint32_t seqno)
{
	pkt nextPacket;
	nextPacket.len = pSize + 12;
	nextPacket.seqno = seqno++;
	nextPacket.ackno = 0;
	nextPacket.cksum = checksum(nextPacket);
	memcpy(nextPacket.data, chunk, pSize);
	return nextPacket;
}

uint16_t myrdtlib::getLen()
{
	uint16_t len = PAYLOAD_SIZE + 12;
	return len;
}


uint16_t myrdtlib::checksum(pkt p)
{
	//initialize checksum to 0
	p.cksum = (uint16_t)0;
	
	char *cHeader=new char[12];
	memcpy(&cHeader, &p.cksum,2);
	memcpy(&cHeader[2], &p.len, 2);
	memcpy(&cHeader[4], &p.ackno, 4);
	memcpy(&cHeader[8], &p.seqno, 4);

	register word32 sum = 0;
	char *buff = cHeader;
	char *addr = buff;
	word32 count = (strlen(buff)-1);
	while (count > 1)
	{
			sum += (word16)*addr;
			addr++;
			count = count - 2;
	}
	//add leftover byte if there is one
	if (count > 0)
	{
			sum = sum + *((byte *) addr);
	}
	//fold to 16 bit number
	while (sum>>16)
	{
			sum = (sum & 0xFFFF) + (sum >> 16);
	}
	
	delete cHeader;
	uint16_t ckSum(sum);
	return ckSum;
}

// takes a string (presumably read by a file) and breaks it into chunks
// returns a C-string array
char** myrdtlib::chunkData(string blob, size_t chunksz)																																																																																																																																						
{
	size_t numchunks, current = 0;
	char** list;

	// determine number of chunks needed to hold the blob
	numchunks = blob.size() / chunksz;
	// allocate enough slots to hold memory locations of the chunksz
	list = (char **) malloc (numchunks * sizeof(char*));

	do
	{		

		size_t this_chunksz = (current == numchunks) ? blob.length() % chunksz : chunksz;

		// allocate the memory for this chunk
		list[current] = (char *) malloc( this_chunksz );	

		// memcpy the block of memory
		memcpy( list[current], blob.c_str() + (current*chunksz), this_chunksz );

	} while( current++ < numchunks );

	return list;

}

#endif //MYRDTLIB_CPP