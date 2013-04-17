// memfn.h
// Various functions having to do with direct memory access


// Print the contents of the memory cell for the number of bytes specified
// Need to cast as char pointer first ?
void printMemory( void *cell, size_t bytes )
{	
	char *block = new char[bytes];
	char *begin = block;
	memcpy( block, cell, bytes );

	const char* end = block + bytes;
	int i = 0;
	while ( block < end )
	{
		printf( "%02x ", *block++ );

		// add a space between each group of 4 codes
		if( ++i % 4 == 0 )
			printf( "  " );

		// line break on each 16 bytes
		if( i % 16 == 0 )
			printf("\n");

	}

	printf("\n");
	delete[] begin;
	
}



