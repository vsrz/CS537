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

static int findInString(string, string);

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

/* consists of the different parts of an HTTP request */
struct HttpRequest
{
	string command;
	string path;
	string protocol;
	vector<string> headers;
	string body;
};

void line(string s)
{
	stringstream ss(s);
	string token;
	string res;
	while( getline( ss, token, ' ' ) )
	{		
		res = token;
		cout << res;
	}
}


HttpRequest *parseRequest(string s)
{
	HttpRequest *http = new HttpRequest;
	stringstream ss( s );
	string token,crlf("\r\n");
	int body = 0;

	/* break string into chunks delimited by whitespace
	   but only read the first 3 chunks to get command section */
	for ( int i = 0; i < 3; ++i )
	{
		string token;
		ss >> token;
		switch ( i )
		{
			case 0:
				http->command = token;
				break;
			case 1:
				http->path = token;
				break;
			case 2:
				http->protocol = token;
				break;
		}		
	}

	/* now stick the rest of the string into the string vector 
		but only if the line isn't blank */
	while( getline( ss, token ) )
	{
		if ( token.length() != 1 ) 
		{
			http->headers.push_back(token);
		}
		else if( body )
		{
			// if content length header sent, read the body now
			getline( ss, token );
			http->body = token;
		}

		// keep flag for content-length
		body += findInString( "Content-Length: ", token);
	}

	return http;
}

// return the position in the string that the needle is found, 0 if not found
static int findInString(string needle, string haystack)
{
        size_t found = haystack.find(needle, 0);
        if( found != std::string::npos )
                return found;
        return -1;

}

/* takes the header list and a header key and returns the value found */
string getHeaderValue(vector<string> h, string header)
{	
	for ( vector<string>::iterator it = h.begin();
		it != h.end();
		++it)
	{
		string &i(*it);
		if(findInString(header + ": ", i) >= 0)
		{
			i = i.substr(i.find(": ")+2);
			return i.substr(0,i.size()-1);
		}
	}
	return string("");
}



int main(int argc, char** argv)
{
	string s = "PUT /ajsdkf.html HTTP/1.1\r\nHost: www.csusm.edu\r\nConnection: keep-alive\r\nContent-Length: 19\r\n\r\nname=ruturajv&sex=m";
	string s2 = "PUT / HTTP/1.1\r\n\r\n";
	HttpRequest *http;
	http = parseRequest(s);
	cout << getHeaderValue(http->headers, "Connection") << endl;
	if ( getHeaderValue ( http->headers, "Connection" )  == string("keep-alive") )
		cout << "Keep conn open" << endl;
	else
		cout << "Close Conn" << endl;
	delete http;
	return 0;

}











