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

#define PAYLOAD_SIZE 100

int main(int argc, char** argv)
{
	string file;
	char** chunks;
	char* packet;


	readFile( file, "sample.txt" );

	chunks = chunkFile( file, PAYLOAD_SIZE );
		
	std::cout << chunks[0] << endl;
	packet = createPacket( chunks[0], PAYLOAD_SIZE, "____________", 12 );
	
	std::cout << packet << endl;

	return 0;

}











