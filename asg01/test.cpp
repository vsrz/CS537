#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <fstream>
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

#define MAXLINE	32768

using namespace std;

const int backlog = 4;
/* Read file and return the file in a string 
 * assume it exists
 */
string readFile(string fn)
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
		
		return content;
	}
	
	return string();
}	
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

/* Sends the date header specified in RFC 2822 */
string getDateTimeRfc2822()
{
	// current date/time based on current system
	time_t now = time(0);
	stringstream ss;
	string str;	
	tm *tm = gmtime(&now);
	
	/* Display format for months and days of the week */
	const char * weekday[] = { "Sun", "Mon", "Tue", "Wed", 
		"Thu", "Fri", "Sat" };
	const char * month [] = { "Jan", "Feb", "Mar", "Apr", "May",
		"Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	
	/* Weekday array format from above */
	cout << weekday[tm->tm_wday] << endl;
	ss << weekday[tm->tm_wday] << ", "  << tm->tm_mday << " ";
	
	/* Month from array, and current year + 1900 */
	ss << month[tm->tm_mon] << " " << (1900 + tm->tm_year) << " ";
	
	/* hr, min, and sec with leading zero check */
	ss << (tm->tm_hour < 10 ? "0" : "") << tm->tm_hour << ":";
	ss << (tm->tm_min < 10 ? "0" : "") << tm->tm_hour << ":";
	ss << (tm->tm_sec < 10 ? "0" : "") << tm->tm_sec;
	ss << " GMT" << "\r\n";
	str = ss.str();
	return str;
}

string fileEndsWith(string fn)
{
	return( fn.substr( fn.find_last_of(".") + 1 ) );


}

string stringToUpper(string s)
{
	string str;
	for( size_t l = 0; l < s.length(); l++ )
		str += toupper(s[l]);
	return str;
}

int main()
{
	string response("");
	cout << response.length() << endl;
	
	return 0;

}