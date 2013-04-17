// rdtip.h
// rdt ip functions

#ifndef RDTSENDER_H
#define RDTSENDER_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "rdtpacket.h"
#include "rdtlib.h"

struct sockaddr_in setIPAddress( char * addr, int port )
{
	struct 	sockaddr_in servaddr;

	bzero( &servaddr, sizeof( servaddr ));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr( addr );
	servaddr.sin_port = htons( port );
	
	return servaddr;
}

int bindSocket () 
{
	int sock = rdt_socket(AF_INET, SOCK_DGRAM, 0);
	if ( sock < 0 )
	{
		std::cout << "error on rdt_socket\ncannot open socket\n";
		exit(-1);
	}
	return sock;
}

#endif



