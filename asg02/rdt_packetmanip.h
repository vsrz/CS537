
// packet manipulator
// used to form rdp packets
#include <iostream>
#include <cstring>


// Resizes and pads the beginning of a packet with header
char* createPacket( char *payload, size_t payloadsz, string header, size_t headersz )
{
	char *newpacket;

	// create the new memory location for the packet contents
	newpacket = (char *) malloc( headersz + payloadsz * sizeof( char ));

	// copy the header into the new string
	memcpy( newpacket, header.c_str(), headersz );
	
	// copy the payload into the new string
	memcpy ( newpacket + headersz, payload, payloadsz );

	return newpacket;



}

// takes a string (presumably read by a file) and breaks it into chunks
// returns a C-string array
char** chunkFile( string blob, size_t chunksz )																																																																																																																																						
{
	size_t numchunks, current = 0;
	char** list;

	// determine number of chunks needed to hold the blob
	numchunks = blob.size() / chunksz;
	// allocate enough slots to hold memory locations of the chunksz
	list = (char **) malloc (numchunks * sizeof(char*));
	
	do
	{		

		size_t this_chunksz = (current == numchunks) ? blob.length() % chunksz - 1 : chunksz;

		// allocate the memory for this chunk
		list[current] = (char *) malloc( this_chunksz );	
		
		// memcpy the block of memory
		memcpy( list[current], blob.c_str() + (current*chunksz), this_chunksz );

	} while( current++ < numchunks );

	return list;

}


