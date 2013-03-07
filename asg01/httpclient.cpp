#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <time.h>
using namespace std;

int portno, reqType;
string httpVer = "1";
struct hostent *server;
string host, connection;
vector<string> references;
bool refCon;

void *conn(void *arg);
static size_t findInString(string needle, string haystack);
void readSock(int sockfd);

bool writeFileToDisk(string file, string filename)
{
        ofstream f;
        f.open(filename.c_str());
        f << file;
        f.close();
        return true;
}

string readBody( int fd, int buflength )
{
	char str;
	string body;

	for (
		size_t i = 0;
		body.size() < (size_t)buflength;
		++i)
		
	{
		if ( recv( fd, &str, 1, 0) < 1) break;
		body.push_back(str);
	}
cout << "here" << endl;
	return body;	


}
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

int getConLen(string headers)
{
	int conLen;
	bool conF = false;
	stringstream ss(headers);
	string token;
	while (ss >> token)
	{
		if (conF)
		{
			istringstream(token) >> conLen;
			break;
		}
		if (token == "Content-Length:")
		{
			conF = true;
		}
	}
	return conLen;
}

void readSock(int sockfd)
{
	string headers = readBuffer(sockfd);
	int conLen = getConLen(headers);
	char buff[2];
	int readIn = 0;
	while (read(sockfd, buff, 1) != 0)
	{
		readIn++;
		if (readIn == conLen)
		{
			cout << "Received all " << conLen << " bytes of file: ";
			break;
		}
	}
}

timespec diff(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

// return the position in the string that the needle is found, 0 if not found
static size_t findInString(string needle, string haystack)
{
	size_t found = haystack.find(needle, 0);

	if( found != std::string::npos )
		return found;
	return -1;
}

int main(int argc, char *argv[])
{
	refCon = false;
	int mode;
	string path;
   	int tCount = 1;
	void *ret;
	
	pthread_t tid;

	if (argc < 2)
	{
			cout << "Usage: %s hostname port" << endl;
			return 0;
	}

        if (argc == 2)
	{
                portno = 80;
	}
	else
	{
	        portno = atoi(argv[2]);

	}
	
	string url = argv[1];
	string delim = "/";
	size_t found = url.find(delim);
	string url_h = url.substr(0, found);
	if (found != std::string::npos)
	{
		server = gethostbyname(url_h.c_str());
		path = url.substr(found, url.length());
		host = url_h;
	}
	else
	{
		server = gethostbyname(argv[1]);
		path = "/";
		host = argv[1];
	}

	bool vinput = false;
	while(!vinput)
	{
		cout << "Enter mode (1 = Load Generator, 2 = Browser): ";
		cin >> mode;
		if (mode == 1 || mode == 2)
		{
			vinput = true;
		}
		else
		{
			cout << "Invalid entry, you must enter 1 or 2." << endl;
		}
	}
	if (mode == 1)
	{
		reqType = 1;
		vinput = false;
		while (!vinput)
		{
			cout << "Enter thread count (1-100): ";
			cin >> tCount;
			if (tCount > 0 && tCount < 101)
			{
				vinput = true;
			}
			else
			{
				cout << "Invalid entry, you must enter a count between 1 and 100." << endl;
			}
		}
	}
	else if (mode == 2)
	{
		vinput = false;
		while(!vinput)
		{
			cout << "Enter request type (1 = GET, 2 = HEAD, 3 = PUT, 4 = DELETE): ";
			cin >> reqType;
			if (reqType == 3)
				cout << "Sorry, PUT has not yet been implemented. Please try a different request. \n" << endl;
			if (reqType == 1 || reqType == 2 || reqType == 4)
			{
				vinput = true;
			}
			else
			{
				cout << "Invalid entry, you must enter 1, 2, 3, or 4." << endl;
			}
		}
	}
	vinput = false;
	while (!vinput)
	{
		cout << "Enter HTTP version: (0 = HTTP/1.0, 1 = HTTP/1.1): ";
		cin >> httpVer;
		if (httpVer == "0")
		{
			vinput = true;
			connection == "close";
		}
		else if (httpVer == "1")
		{
			vinput = true;
			connection = "keep-alive";
		}
		else
		{
			cout << "Invalid entry, you must enter 0 or 1." << endl;
		}
	}
	
	timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);
	

	for (int i = 0; i < tCount; i++)
	{
		if (pthread_create(&tid, NULL, conn, (void *)&path) != 0)
			cout << "error creating thread" << endl;
	}

	if (pthread_join(tid, &ret) != 0)
	{
		cout << "error joining thread" << endl;
		return 0;
	}
	
	bool mThreads = false;
	while (!references.empty())
	{
		refCon = true;
		mThreads = true;
		path = "/"+references.back();
		references.pop_back();
		if (pthread_create(&tid, NULL, conn, (void *)&path) != 0)
			cout << "error creating thread" << endl;
	}
	if (mThreads)
	{
		if (pthread_join(tid, &ret) != 0)
		{
			cout << "error joining thread" << endl;
			return 0;
		}
	}
	
	clock_gettime(CLOCK_REALTIME, &end);
    cout << "Runtime: " << diff(start, end).tv_nsec << " nanoseconds, " << diff(start, end).tv_sec << " seconds\n";
    return 0;
}

void *conn(void *arg)
{
	char * ret = NULL;
	string path = *(string*) (arg);
		int sockfd, n;
        struct sockaddr_in serv_addr;
		

        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
        {
                cout << "Error opening socket" << endl;
                return NULL;
        }


        if (server == NULL)
        {
                cout << "Error: no such host." << endl;
                return NULL;
        }

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        {
                cout << "Error connecting" << endl;
                return NULL;
        }

	string req_str;
	switch(reqType)
	{
		case 1:
			req_str = "GET "+path+" HTTP/1."+httpVer+"\r\nHost: "+host+"\r\nConnection: "+connection+"\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\r\n";
			break;
		case 2:
			req_str = "HEAD "+path+" HTTP/1."+httpVer+"\r\nHost: "+host+"\r\n\r\n";
			break;
		case 3:
			req_str = "PUT "+path+" HTTP/1."+httpVer+"\r\nHost: "+host+"\r\n\r\n";
			break;
		case 4:
			req_str = "DELETE "+path+" HTTP/1."+httpVer+"\r\nHost: "+host+"\r\n\r\n";
			break;
	}
        char *req = new char[req_str.size() + 1];
        req[req_str.size()] = 0;
        memcpy(req, req_str.c_str(), req_str.size());
        n = write(sockfd, req, strlen(req));
        if (n < 0)
        {
                cout << "Error writing to socket" << endl;
                return NULL;
        }
		string headers = readBuffer(sockfd);
		int conLen = getConLen(headers);
		string content, temp, refPath;
		char buff[2];
		int readIn = 0;
		while (read(sockfd, buff, 1) != 0)
		{
			string curr(buff);
			content += curr;
			readIn++;
			if (readIn == conLen)
				break;
		}
		if(!refCon)
			cout << content;
		stringstream ss(content); //stream for finding content length
		while (getline(ss, temp, '\n'))
		{
			if (findInString("src", temp) != std::string::npos)
			{
				stringstream ssref(temp);
				getline(ssref, refPath, '\"'); //removes the part of the line before the path
				getline(ssref, refPath, '\"'); //gets the path
				references.push_back(refPath);
			}
		}
		if (!refCon)
			cout << "Ref path is " << refPath << endl;
		string truePath;
		if (path == "/")
			truePath = "/index.html";
		else
			truePath = path;
		cout << "Received all " << conLen << " bytes of file " << truePath << endl;
		if (httpVer == "1")
		{
			while(!references.empty())
			{
				string refPath = references.back();
				references.pop_back();
				req_str = "GET /"+refPath+" HTTP/1."+httpVer+"\r\nHost: "+host+"\r\nConnection: "+connection+"\r\nAccept: */*\r\n\r\n";
				char *req = new char[req_str.size() + 1];
				req[req_str.size()] = 0;
				memcpy(req, req_str.c_str(), req_str.size());
				n = write(sockfd, req, strlen(req));
				if (n < 0)
				{
						cout << "Error writing to socket" << endl;
						return NULL;
				}
				readSock(sockfd);
				cout << refPath << endl;
				delete req;
			}
		}
		delete req;
	pthread_exit(ret);
}



