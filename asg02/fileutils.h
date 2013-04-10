#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>

using namespace std;

namespace fileutils
{

/* write string stream to disk */
bool writeFileToDisk(string file, string filename)
{
        ofstream f;
        f.open(filename.c_str());
        f << file;
        f.close();
        return true;
}


/* gets the extension of a filename */
string getFileExtension(string fn)
{
	return( fn.substr( fn.find_last_of(".") + 1 ) );
}

/* converts file to uppercase */
string stringToUpper(string s)
{
	string str;
	for( size_t l = 0; l < s.length(); l++ )
		str += toupper(s[l]);
	return str;
}

/* returns bool based on file existing */
bool fileExists(string fn)
{
	ifstream in;
	in.open(fn.c_str(), ios::binary);
	if(in.is_open())
	{
		in.close();
		return true;
	}
	return false;

}

/* Read file and return the file in a string 
 * assume it exists */
void readFile(string &s, string fn)
{
	ifstream f(fn.c_str(), ios::in | ios::binary);
    
	if( f )
	{
		string content;
		
		/* Jump to the end and grab the length */
		f.seekg( 0, ios::end );
		content.resize( f.tellg() );
		f.seekg( 0, ios::beg );
		
		/* read close and return the file */
		f.read(&content[0], content.size());
		f.close();		
		
		s += content;
	}
		
}	

} // end namespace
