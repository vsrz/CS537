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

#include "fileutils.h"
#include "rdt_packetmanip.h"

using namespace fileutils;
using namespace std;

typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int

bool sqn = false;

#define PAYLOAD_SIZE 50

struct packet {
	uint16_t cksum; /* Ack and Data */
	uint16_t len;   /* Ack and Data */
	uint32_t ackno; /* Ack and Data */
	uint32_t seqno; /* Data only */
	char data[PAYLOAD_SIZE]; /* Data only; Not always 500 bytes, can be less */
};

typedef struct packet packet_t;

void testPacketChunking()
{
	string file;
	char** chunks;
	char* packet;


	readFile( file, "sample.txt" );
	chunks = chunkFile( file, PAYLOAD_SIZE );

	//packet = createPacket( chunks[2], PAYLOAD_SIZE, "____________", 12 );

	std::cout << packet << endl;


}

uint16_t getLen()
{
	uint16_t len = PAYLOAD_SIZE + 12;
	return len;
}

uint32_t getSeqno()
{
	uint32_t seqno;
	if (sqn)
	{
			seqno = 0;
			sqn = false;
	}
	else
	{
			seqno = 1;
			sqn = true;
	}
	return seqno;
}

uint16_t checksum()
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


void testAsciiCh(  )
{
	std::basic_string<unsigned char> s;
	s = convertInt16ToAscii( 1512 );
	cout << dec << "hi: " << (int)s[0] << " lo: " << (int)s[1] << endl;

}

int main(int argc, char** argv)
{
	void *payload;
	const int PACKET_SIZE = sizeof(uint16_t)*6 + PAYLOAD_SIZE*sizeof(char);
	packet p,s;

	// allocate mem
	payload = malloc(PACKET_SIZE);

	// zero out mem space
	memset(payload, '\0', PACKET_SIZE);

	// setup the packet
	p.len = getLen();
	p.ackno = (uint32_t) 0;
	p.seqno = getSeqno();
	p.cksum = checksum();

	char data[] = "sample data stuff";	
	memcpy( p.data, &data, PAYLOAD_SIZE);

	printf("Sender\npck: %d\nplen: %d\npackno: %d\npdata: %s\n\n", (int) p.cksum, (int) p.len, (int) p.ackno, p.data);

	memcpy ( payload, &p, PACKET_SIZE );

	// send payload

	memcpy( &s, payload, PACKET_SIZE );

	printf("Receiver\nsck: %d\nslen: %d\nsackno: %d\nsdata: %s\n", (int) s.cksum, (int) s.len, (int) s.ackno, s.data);

	return 0;

}