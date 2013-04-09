
// packet manipulator
// used to form rdp packets
#include <iostream>
#include <cstring>


// Resizes and pads the beginning of a packet with header
char* wrapPacket( char *packet, size_t packetsz, string header, size_t headersz )
{
	char *newpacket;

	// create the new memory location for the packet contents
	newpacket = (char *) malloc( headersz + packetsz * sizeof( char ));

	// copy the header into the new string
	memcpy( newpacket, header.c_str(), headersz );
	
	// copy the payload into the new string
	memcpy ( newpacket + headersz, packet, packetsz );

	return newpacket;



}

