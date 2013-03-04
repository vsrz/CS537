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

#include "fileutils.h"
#include "dateutils.h"

#define MAXLINE	32768

using namespace std;
using namespace fileutils;
using namespace dateutils;

const int backlog = 4;


/* consists of the different parts of an HTTP request */
struct HttpRequest
{
	string command;
	string path;
	string protocol;
	vector<string> headers;
	string body;

};


// return the position in the string that the needle is found, 0 if not found
int findInString(string needle, string haystack)
{
	size_t found;
	found = haystack.find(needle);
	if( found != string::npos )
		return found+1;
	return 0;

}

/* takes the header list and a header key and returns the value found */
string getHeaderValue(vector<string> h, string header)
{	
	for ( vector<string>::iterator it = h.begin();
		it != h.end();
		++it)
	{
		string i(*it);
		if(findInString(header, i) == 1)
		{
			i = i.substr(findInString(" ", i),i.size()-1);
			i.erase(i.find_last_not_of(" \n\r\t")+1);
			return i;
		}
	}
	return string("");
}

void responseServerHeader(string &s)
{
	s += "Server: CS537_POWVIL/1.0\r\n";
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


/* Send the 400 page */
void responseHttpBadRequest(string &s, const string protocol)
{
	s += protocol + "400 Bad Request\r\n";
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

void httpGet(HttpRequest *http, int fd)
{
    string request, resp_header(""), resp_body("");

	// Strip the slash off the path
	if( findInString("/",http->path) == 1 )
		http->path = http->path.substr( 1,http->path.length() );

	/* default to index.html if none provided */
    if (http->path == "")
    	http->path = "index.html";
    
	
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

/* handle and respond to an http delete request */
void httpDelete(HttpRequest *http, int fd)
{

}

/* handle and respond to an http put request */
void httpPut(HttpRequest *http, int fd)
{

}

/* handle and respond to an http head request */
void httpHead(HttpRequest *http, int fd)
{
    string request, resp_header(""), resp_body("");

	// Strip the slash off the path
	if( findInString("/",http->path) == 1 )
		http->path = http->path.substr( 1,http->path.length() );

	/* default to index.html if none provided */
    if (http->path == "")
    	http->path = "index.html";
    
	
	/* If file doesn't exist, display a 404 error */
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

void *clientHandler(void *arg)
{

    char str;
    vector<string> cli_buf;
    //size_t i = 0;
	string request;
    bool conn = true;
    int fd = *(int*)(arg);
    cout << "Client accepted" << endl;
	HttpRequest *http;
    //bzero(str,MAXLINE);

	/* maintain conn */
	while ( conn )
	{
		/* read buffer one line at a time until you get two CRLF's  */
		for ( 
			size_t i = 0; 
			(request.size() >= 4 ? 
				request.compare(request.size() -4, 4, "\r\n\r\n")  : 1); 
			i++)
		{
			if ( recv( fd, &str, 1, 0) < 1)  break;
				request.push_back(str);			
		} // end for
	
		/* Send the entire buffer to the read processor
  		   and return a pointer to an http request object */
		http = parseRequest(request);

		// Determine the request type and move forward
		if ( http->command == "GET" )
			httpGet(http, fd);
		else if ( http->command == "DELETE" )
			httpDelete(http, fd);
		else if ( http->command == "HEAD" )
			httpHead(http, fd);
		else if ( http->command == "PUT" )
			httpPut(http, fd);
		else 
			httpError(http, fd);

		if ( getHeaderValue( http->headers, "Connection" ) != "keep-alive" )
			conn = false;
			
#ifdef DEBUG
		cout << "command: " << http->command << endl;
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
	cout << "Connection closed" << endl;
	close(fd);

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

