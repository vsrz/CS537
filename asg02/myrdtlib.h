#ifndef _MYRDTLIB_H
#define _MYRDTLIB_H

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
using namespace std;

typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int
#define PAYLOAD_SIZE 1500

class myrdtlib
{
private:
	struct pkt {
		uint16_t cksum; /* Ack and Data */
		uint16_t len;   /* Ack and Data */
		uint32_t ackno; /* Ack and Data */
		uint32_t seqno; /* Data only */
		char data[PAYLOAD_SIZE]; /* Data only; Not always 1500 bytes, can be less */
	};
	typedef struct pkt pkt_t;
	pkt genPacket(char* chunk, int pSize, uint32_t seqno);
	uint16_t checksum(pkt p);
	bool okToSend(uint32_t seqno, uint32_t lastACK);
	pkt readPacket(int sockfd);
	uint32_t getLastACK(int sockfd);
	uint16_t getLen();
	char** PacketChunking(string file, int &lpSize);
	char** chunkData(string blob, size_t chunksz);
	
public:
	int rdt_socket(int address_family, int type, int protocol);
	int rdt_bind(int socket_descriptor, const struct sockaddr *local_address, socklen_t address_length);
	int rdt_sendto(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *destination_address, int address_length);
	int rdt_recv(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *from_address, int *address_length);
	int close(int fildes);
};	//myrdtlib

#endif //_MYRDTLIB_H