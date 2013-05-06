
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <string>
#include <sstream>
#include "fileutils.h"

using namespace std;

int size = 0;

// reads the file provided and punches it into matrix
void loadMatrix( std::string filename )
{
    ifstream f(filename.c_str(), ios::in | ios::binary);

	// preload the file 
	std::string fdata;
	fileutils::readFile( fdata, filename );

	/**
	 * Determine the size of the matrix by counting the newlines
	 * if the file doesn't end with a newline, that means we need
	 * to increment the array by 1
	 */
	int size = std::count( fdata.begin(), fdata.end(), '\n' );
	if( *fdata.rbegin() != '\n' )
	{
		size++;
	}

	// create the data structure
	int **matrix = new int *[size];
	for( int i = 0; i < size; ++i )
	{
		matrix[i] = new int[size];
	}

	// now, fill the array
    if ( f )
    {
		for( int y = 0; y < size; ++y )
		{
			for( int x = 0; x < size; ++x )
			{
				f >> matrix[x][y];
			}
		}
    }
	
	// now, print the array
	for( int y = 0; y < size; ++y )
	{
		for( int x = 0; x < size; ++x )
		{
			std::cout << matrix[x][y] << " ";
		}
		std::cout << std::endl;
	}
}


int main( int argc, char **argv )
{
	// Is the input file specified?
	if( argc < 2 )
	{
		std::cout << "Format is " << argv[0] << " <input file> " << std::endl;
		return 1;
	}

	// Check if the file exists
	std::string filename( argv[1] );
	if(! fileutils::fileExists( filename ) )
	{
		std::cout << "The file you specified could not be read." << std::endl;
		return 1;
	}

	// load the file
	loadMatrix( filename );

	return 0;
}

