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

#include "fileutils.h"
#include "rdt_packetmanip.h"

using namespace fileutils;
using namespace std;

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
	
	packet = createPacket( chunks[2], PAYLOAD_SIZE, "____________", 12 );
	
	std::cout << packet << endl;


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
	const int PACKET_SIZE = sizeof(uint16_t)*4 + PAYLOAD_SIZE*sizeof(char);
	packet p,s;

	// allocate mem
	payload = malloc(PACKET_SIZE);

	// zero out mem space
	memset(payload, '\0', PACKET_SIZE);

	// setup the packet
	p.cksum = (uint16_t) 1;
	p.len = (uint16_t) 2;
	p.ackno = (uint32_t) 3;
	p.seqno = (uint32_t) 4;

	char data[] = "sample data stuff";	
	memcpy( p.data, &data, PAYLOAD_SIZE);

	printf("Sender\npck: %d\nplen: %d\npackno: %d\npdata: %s\n\n", (int) p.cksum, (int) p.len, (int) p.ackno, p.data);

	memcpy ( payload, &p, PACKET_SIZE );

	// send payload
	
	memcpy( &s, payload, PACKET_SIZE );

	printf("Receiver\nsck: %d\nslen: %d\nsackno: %d\nsdata: %s\n", (int) s.cksum, (int) s.len, (int) s.ackno, s.data);
	
	return 0;

}












