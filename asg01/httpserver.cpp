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
#include <stdlib.h>
#include <ctime>

#include "fileutils.h"
#include "dateutils.h"

#define MAXLINE	32768

//#define DEBUG
#define LOG_CONSOLE

using namespace std;
using namespace fileutils;
using namespace dateutils;

const int backlog = 4;
const int BIND_PORT = 7777;

#ifdef LOG_CONSOLE
static int conn_count = 0;
#endif

/* consists of the different parts of an HTTP request */
struct HttpRequest
{
	string method;
	string path;
	string protocol;
	vector<string> headers;
	string body;

};


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




void responseServerHeader(string &s)
{
	s += "Server: CS537/1.0\r\n";
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
	s += protocol + " 404 Not Found\r\n";
}


/* Send the 400 page */
void responseHttpBadRequest(string &s, const string protocol)
{
	s += protocol + " 400 Bad Request\r\n";
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

/* handle and respond to an invalid http request */
void httpError(HttpRequest *http, int fd)
{
	string resp_header(""), resp_body("");

	// force an error page
	http->path = "400.html";
	responseHttpBadRequest(resp_header, http->protocol);
	responseContentType(resp_header, http->path);
	readFile(resp_body, http->path);
	responseContentLength(resp_header, resp_body);
	getDateTimeRfc2822(resp_header);
	responseServerHeader(resp_header);
	sendHttpResponse(resp_header + "\r\n" + resp_body, fd);
	
}

/* handle and respond to an http delete request */
void httpDelete(HttpRequest *http, int fd)
{
	string request, resp_header(""), resp_body("");
		
	// Strip the starting slash off the path
	if( findInString("/",http->path) == 0 )
		http->path = http->path.substr( 1 );

#ifdef LOG_CONSOLE
        cout << "DELETE request on: " << http->path << endl;
#endif
	if( remove( http->path.c_str() ) == 0 )
	{
		responseHttpOk(resp_header, http->protocol);
	} 
	else
	{
		responseHttpNotFound(resp_header, http->protocol);
	}
	
	getDateTimeRfc2822(resp_header);
	responseServerHeader(resp_header);
	sendHttpResponse(resp_header + "\r\n", fd);
	
}

void httpGet(HttpRequest *http, int fd)
{
    string request, resp_header(""), resp_body("");

	// Strip the slash off the path
	if( findInString("/",http->path) == 0 )
		http->path = http->path.substr( 1,http->path.length() );

	/* default to index.html if none provided */
	if (http->path == "")
    		http->path = "index.html";
    
#ifdef LOG_CONSOLE
		cout << "GET request on: " << http->path << endl;
#endif
	
	/* If file doesn't exist, display a 404 error */
	if(fileExists(http->path))
		responseHttpOk(resp_header, http->protocol);		
	else
	{
		responseHttpNotFound(resp_header, http->protocol);
		http->path = "404.html";
	}
	
	getDateTimeRfc2822(resp_header);
	responseContentType(resp_header, http->path);
	readFile(resp_body, http->path);
	responseContentLength(resp_header, resp_body);
	sendHttpResponse(resp_header + "\r\n" + resp_body, fd);		
	
}

/* breaks a string object into an http request struct */
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
				http->method = token;
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

/* handle and respond to an http head request */
void httpHead(HttpRequest *http, int fd)
{
    string request, resp_header(""), resp_body("");

	// Strip the slash off the path
	if( findInString("/",http->path) == 0 )
		http->path = http->path.substr( 1,http->path.length() );

    if (http->path == "")
    	http->path = "index.html";
    
#ifdef LOG_CONSOLE
		cout << "HEAD request on: " << http->path << endl;
#endif
	
	/* If file doesn't exist, return a 404 error */
	if( fileExists( http->path ) )
	{
		responseHttpOk( resp_header, http->protocol );		
		getLastModifiedRfc2822( resp_header );
		responseServerHeader( resp_header );
		readFile(resp_body, http->path);
		responseContentType( resp_header, http->path );
		responseContentLength( resp_header, resp_body );
	}
	else
	{
		responseHttpNotFound( resp_header, http->protocol );
		http->path = "404.html";
		responseContentType( resp_header, http->path );
	}
	
	getDateTimeRfc2822(resp_header);

	// Send the content length, but not the actual document
	sendHttpResponse(resp_header + "\r\n", fd);		
	
}

/* reads buffer up to the input length */
string readBody( int fd, int buflength )
{
	char str;
	string body;

	for (
		size_t i = 0;
		body.size() < (size_t)buflength;
		i++)
	{
		if ( recv( fd, &str, 1, 0) < 1) break;
		body.push_back(str);
	}

	return body;	


}
/* reads input buffer from the network, terminating on double CRLF */
string readBuffer(int fd) 
{

	char str;
	string request;

	for ( 
		size_t i = 0; 
		(request.size() >= 4 ? 
			request.compare(request.size() -4, 4, "\r\n\r\n")  : 1); 
		i++)
	{
		if ( recv( fd, &str, 1, 0) < 1)  break;
			request.push_back(str);			
	} // end for
	
	return request;

}

/* handle and respond to an http put request */
void httpPut(HttpRequest *http, int fd)
{
	size_t psize;
	string str, resp_header("");

        // Strip the slash off the path
        if( findInString("/",http->path) == 0 )
                http->path = http->path.substr( 1,http->path.length() );

	str = getHeaderValue( http->headers, "Content-Length" );

	if ( str != "" )
	{
		stringstream (str) >> psize;
		str = readBody(fd, psize);
		responseHttpOk(resp_header, http->protocol);	
		getDateTimeRfc2822(resp_header);
		if( writeFileToDisk(str, http->path) )		
			sendHttpResponse(resp_header + "\r\n", fd);		
		else
			httpError(http, fd);
	}
	else
		httpError(http, fd);
		
}

void *clientHandler(void *arg)
{

    vector<string> cli_buf;
    //size_t i = 0;
	string request;
    bool conn = true;
    int fd = *(int*)(arg);
#ifdef LOG_CONSOLE
    cout << " (" << ++conn_count << ") " << endl;
#endif
	HttpRequest *http;
    //bzero(str,MAXLINE);

	/* maintain conn */
	while ( conn )
	{
		request = readBuffer(fd);

		/* Send the entire buffer to the read processor
  		   and return a pointer to an http request object */
		http = parseRequest(request);
#ifdef DEBUG
		cout << getHeaderValue(http->headers, "Content-Length") << endl;
		for ( vector<string>::iterator it = http->headers.begin();
					it != http->headers.end();
					++it)
		{
			cout << *it << endl;
		}

#endif

		if ( http->method == "GET" )
			httpGet(http, fd);
		else if ( http->method == "DELETE" )
			httpDelete(http, fd);
		else if ( http->method == "HEAD" )
			httpHead(http, fd);
		else if ( http->method == "PUT" )
			httpPut(http, fd);
		else 
			httpError(http, fd);

		if ( getHeaderValue( http->headers, "Connection" ) != "keep-alive" )
			conn = false;

			
#ifdef DEBUG
		cout << "command: " << http->method << endl;
		cout << "path: " << http->path << endl;
		cout << "protocol: " << http->protocol << endl; 
		for ( vector<string>::iterator it = http->headers.begin();
				it != http->headers.end();
				++it)
		{
			cout << *it << endl;
		}
		cout << "body: " << http->body;
#endif
		request = "";
		delete http;

	} // end while( conn )
#ifdef LOG_CONSOLE
	cout << "Client disconnected (" << --conn_count << ") " << endl;
#endif
	close(fd);

    return NULL;
}


int main(int argc, char *argv[])
{

	int	listenfd, connfd;
    pthread_t tid;
	socklen_t clilen;
	char bind_addr[] = "144.37.205.10";
	int bind_port = BIND_PORT;
	struct 	sockaddr_in cliaddr, servaddr;
#ifdef LOG_CONSOLE
	char client_ip[16];
#endif

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		fprintf(stderr, "Error unable to create socket, errno = %d (%s) \n",
		errno, strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family 	   = AF_INET;

	/* determine if argv passed an IP, if so, use it instead */
	if( argc > 1 )
		servaddr.sin_addr.s_addr   = inet_addr(argv[1]);
	else
		servaddr.sin_addr.s_addr   = inet_addr(bind_addr);

	if( argc > 2 )
		servaddr.sin_port          = htons(atoi(argv[2]));
	else
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
#ifdef LOG_CONSOLE
		inet_ntop(AF_INET, &cliaddr.sin_addr, client_ip, 16);
		cout << "Client connected: " << client_ip;
#endif
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

