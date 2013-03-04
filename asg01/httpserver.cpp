#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
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
 * assume it exists
 */
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

/* Returns current date header specified in RFC 2822 */
void getDateTimeRfc2822(string &s)
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
		
	/* http header */
	ss << "Date: ";

	/* Weekday array format from above */
	ss << weekday[tm->tm_wday] << ", "  << tm->tm_mday << " ";
	
	/* Month from array, and current year + 1900 */
	ss << month[tm->tm_mon] << " " << (1900 + tm->tm_year) << " ";
	
	/* hr, min, and sec with leading zero check */
	ss << (tm->tm_hour < 10 ? "0" : "") << tm->tm_hour << ":";
	ss << (tm->tm_min < 10 ? "0" : "") << tm->tm_hour << ":";
	ss << (tm->tm_sec < 10 ? "0" : "") << tm->tm_sec;
	ss << " GMT" << "\r\n";
	s += ss.str();
	
}

/* Puts the response onto the wire */
void sendHttpResponse(const string response, int fd)
{
	write(fd, response.c_str(), response.length());
}

/* Replies with HTTP OK based on protocol provided */
void responseHttpOk(string &s, const string protocol)
{
	s += protocol + " 200 OK\r\n";
}

/* Send the built in 404 page */
void responseHttpNotFound(string &s, const string protocol)
{
	s += protocol + "404 Not Found\r\n";
}

/* Return HTTP Header "Content-Type" */
void responseContentType ( string &s, string filename, string charset = "UTF-8")
{
	string str = "Content-Type: ";	
	string ext;
	
	ext = getFileExtension(filename);
	ext = stringToUpper(ext);
	
	if( ext == "JPG" || ext == "JPEG" )
		str += "image/jpeg";
	else if ( ext == "PNG" ) 
		str += "image/png";
	else if ( ext == "GIF" ) 
		str+= "image/gif";
	else if ( ext == "BMP" ) 
		str += "image/bmp";
	else if ( ext == "HTML"  || ext == "HTM" ) 
		str += "text/html";
	else if ( ext == "LOG"  || ext == "TXT" ) 
		str += "text/plain";
	else if ( ext == "XML" ) 
		str += "text/xml";
	else if ( ext == "JS" ) 
		str += "text/javascript";
	else if ( ext == "CSS" ) 
		str += "text/css";
	else 
		str += "application/octet-stream";

	
	s +=  str + "; " + charset + "\r\n";

}

void responseContentLength( string &s, string body )
{
	stringstream ss;
	int length = body.size();
	s += "Content-Length: ";
	ss << length;
	s += ss.str();
	s += "\r\n";
}

void httpGet(vector<string> s, int fd)
{
    string request, path, proto, header(""), body("");
    request = s[0].substr(0,3);
    path = s[0].substr(5,s[0].length() - 14);
    proto = s[0].substr(s[0].length() - 8,s[0].length());

	/* default to index.html if none provided */
    if (path == "")
    	path = "index.html";
    
	/* If file doesn't exist, display a 404 error */
	if(fileExists(path))
		responseHttpOk(header, proto);		
	else
	{
		responseHttpNotFound(header, proto);
		path = "404.html";
	}
	
	getDateTimeRfc2822(header);
	responseContentType(header, path);
	readFile(body, path);
	responseContentLength(header, body);
	sendHttpResponse(header + "\r\n" + body, fd);		
	
}

void *clientHandler(void *arg)
{

    char str[MAXLINE];
    vector<string> cli_buf;
    int  n;
    bool conn = true;
    int  fd = *(int*)(arg);
    cout << "Client accepted" << endl;

    while (conn) {
        bzero(str,MAXLINE); 
        if ((n = read(fd, str, MAXLINE)) == 0) {
            close (fd);
            conn = false;
	    cout << "Client disconnected.\n";
	    return NULL;
        }

	cli_buf.clear();
	string req(str);
	stringstream ss(req);
	string item;
	while (getline(ss, item, '\n'))
	{
		cli_buf.push_back(item.substr(0, item.length()-1));
	}


	if (cli_buf[0].substr(0, 3) == "GET")
	{
		httpGet(cli_buf, fd);
	}

	cout << "clibuf: " << cli_buf[0] << endl;

        
    }
    return NULL;
}

int main(int argc, char *argv[])
{

	int	listenfd, connfd;
        pthread_t tid;
	socklen_t clilen;
	char bind_addr[] = "144.37.205.10";
	int bind_port = 7777;
	struct 	sockaddr_in cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		fprintf(stderr, "Error unable to create socket, errno = %d (%s) \n",
                errno, strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family 	   = AF_INET;
	servaddr.sin_addr.s_addr   = inet_addr(bind_addr);
	servaddr.sin_port          = htons(bind_port);

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        fprintf(stderr, "Error binding to socket, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;

	}

	if (listen(listenfd, backlog) == -1) {
        fprintf(stderr, "Error listening for connection request, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;
	}


	while (1) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0 ) {
			if (errno == EINTR)
				continue;
			else {
                fprintf(stderr, "Error connection request refused, errno = %d (%s) \n",
                        errno, strerror(errno));
			}
		}

	        if (pthread_create(&tid, NULL, clientHandler, (void *)&connfd) != 0) {
	           fprintf(stderr, "Error unable to create thread, errno = %d (%s) \n",
	                   errno, strerror(errno));
	        }

	}
}

