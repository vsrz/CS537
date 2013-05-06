/**
 *
 * ./LSR <source> <destination> <filename>
 *
 * Input filename should be a matrix of even cols/rows
 * with the distances of the algorithm
 *
 * Must be a valid adjcency matrix such as:
 * -1 3 6 -1
 * 3 -1 2 7
 *  6 2 -1 1
 * -1 7 1 -1
 */

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include "pQueue.h"
#include "node.h"
#include "fileutils.h"
using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 4)
    {
        cout << "Usage: LSR <source> <destination> <file>";
        return -1;
    }
	
	string filename(argv[3]);
    ifstream f(filename.c_str(), ios::in | ios::binary);

	// preload the file 
	std::string fdata;
	fileutils::readFile( fdata, filename );

	/**
	 * Determine the size of the map by counting the newlines
	 * if the file doesn't end with a newline, that means we need
	 * to increment the array by 1
	 */
	int size = std::count( fdata.begin(), fdata.end(), '\n' );
	if( *fdata.rbegin() != '\n' )
	{
		size++;
	}

	// create the data structure
	int **map = new int *[size];
	for( int i = 0; i < size; ++i )
	{
		map[i] = new int[size];
	}

	// now, fill the array
    if ( f )
    {
		for( int y = 0; y < size; ++y )
		{
			for( int x = 0; x < size; ++x )
			{
				f >> map[x][y];
			}
		}
    }

	pQueue hops;
	pQueue known;
	hops.initialize(size);
	hops.setSource(atoi(argv[1]));
	for (int i=0; i<size; i++)
	{
		node parent = hops.deleteMin();
		known.insert(parent);
		for (int j=0; j<size; j++)
		{
			if (map[parent.getName()][j] != -1)
			{
				int fringeDist = (parent.getDistance() + map[parent.getName()][j]);
				if (fringeDist < hops.getNodeDist(j) || hops.getNodeDist(j) == -1)
				{
					hops.updateNode(j, parent.getName(), fringeDist);
				}
			}
		}
	}
	int dest = atoi(argv[2]);
	cout << "The minimum distance from node " << argv[1] << " to " << dest << " is " << known.getNodeDist(dest) << endl;
	
	return 0;
}