//rdtSender.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
using namespace std;

/*
int rdt_socket(int, int, int);
int rdt_bind(int, const struct sockaddr*, socklen_t);
int rdt_sendto(int, char*, int, int, sockaddr*, int);
int rdt_recv(int, char*, int, int, sockaddr*, int*);
int close(int);
*/

typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int
#define PAYLOAD_SIZE 1500

//class myrdtlib
//private:
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
	
//public:
	int rdt_socket(int address_family, int type, int protocol);
	int rdt_bind(int socket_descriptor, const struct sockaddr *local_address, socklen_t address_length);
	int rdt_sendto(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *destination_address, int address_length);
	int rdt_recv(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *from_address, int *address_length);
	int close(int fildes);

int rdt_socket(int address_family, int type, int protocol)
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	return sockfd;
}

int rdt_bind(int socket_descriptor, const struct sockaddr *local_address, socklen_t address_length)
{
	int bindResult = bind(socket_descriptor,(struct sockaddr *)&local_address,address_length);
	return bindResult;
}

int rdt_recv(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *from_address, int *address_length)
{
	socklen_t from_length = *address_length;
	int n = recvfrom(socket_descriptor,buffer,buffer_length,0,(struct sockaddr *)&from_address,&from_length);
	return n;
}

int close(int fildes)
{
	close(fildes);
	return 0;
}

int rdt_sendto(int socket_descriptor, char *buffer, int buffer_length, int flags, struct sockaddr *destination_address, int address_length)
{
	uint32_t sqn = 0;
	int sockfd = socket_descriptor;
	string file(buffer);
	int bufLen = buffer_length;
	int lpSize;
	//struct sockadder_in = destination_address;
	
	char** chunks = PacketChunking(file, lpSize);
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

bool okToSend(uint32_t seqno, uint32_t lastACK)
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

pkt readPacket(int sockfd)
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

uint32_t getLastACK(int sockfd)
{
	pkt lastPacket = readPacket(sockfd);
	uint32_t lastACK;
	char buf[8];
	memcpy(&lastPacket, &buf, 8); //copy the whole pkt into the buf
	memcpy(&buf[4], &lastACK, 4); //copy just the ACK field into the uint32_t
	return lastACK;
}

char** PacketChunking(string file, int &lpSize)
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



pkt genPacket(char* chunk, int pSize, uint32_t seqno)
{
	pkt nextPacket;
	nextPacket.len = pSize + 12;
	nextPacket.seqno = seqno++;
	nextPacket.ackno = 0;
	nextPacket.cksum = checksum(nextPacket);
	memcpy(nextPacket.data, chunk, pSize);
	return nextPacket;
}

uint16_t getLen()
{
	uint16_t len = PAYLOAD_SIZE + 12;
	return len;
}


uint16_t checksum(pkt p)
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
char** chunkData(string blob, size_t chunksz)																																																																																																																																						
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
	
#define MAXLINE 80



int main(int argc, char *argv[])
{
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


	sockfd = rdt_socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1) 
	{
		perror("socket error");
		exit(-1);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));

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
		
		char *cdata = new char[sdata.size()+1];
		cdata[sdata.size()] = 0;
		memcpy(cdata, sdata.c_str(), sdata.size());
		if (rdt_sendto(sockfd, (char*)cdata, sizeof(cdata), 0, (struct sockaddr *)&servaddr, servlen) < 0) 
		{
			perror("sendto error");
			return -1;
		}

		n = rdt_recv(sockfd, ptime, MAXLINE, 0, (struct sockaddr *)&servaddr, (int*)&servlen);
		fputs(ptime, stdout);
		delete cdata;
		sleep(1);
	}
	return 0;
}
