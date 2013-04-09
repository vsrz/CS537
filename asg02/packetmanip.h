
// packet manipulator
// used to form rdp packets

#include <cstring>



void padPacket( char &packet, size_t headersz )
{

	// creating the padding for the 
	padding = (char *) malloc (  sizeof( char ));

	memset(padding, " ");

}