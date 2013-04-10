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

#define PAYLOAD_SIZE 4

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
	testAsciiCh();

	return 0;

}












