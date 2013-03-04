#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
using namespace std;

int portno, reqType;
string httpVer = "1";
string path;
struct hostent *server;
string host;

void *conn(void *arg);

int main(int argc, char *argv[])
{
	int mode;
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
	char delim = '/';
	size_t found = url.find(delim);

	if (found != std::string::npos)
	{
		string url_h = url.substr(0, found);
		server = gethostbyname(url_h.c_str());
		path = url.substr(found, url.length()-1);
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
			if (reqType == 1 || reqType == 2 || reqType == 3 || reqType == 4)
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
		if (httpVer == "0" || httpVer == "1")
		{
			vinput = true;
		}
		else
		{
			cout << "Invalid entry, you must enter 0 or 1." << endl;
		}
	}
	

	
	for (int i = 0; i < tCount; i++)
	{
		if (pthread_create(&tid, NULL, conn, (void *)&reqType) != 0)
			cout << "error creating thread" << endl;
	}

	if (pthread_join(tid, &ret) != 0)
	{
		cout << "error joining thread" << endl;
		return 0;
	}

        cout << "\n";
        return 0;
}

void *conn(void *arg)
{
	char * ret = NULL;
	int reqType = *(int*) (arg);
        int sockfd, n;
        struct sockaddr_in serv_addr;

        char buffer[256];


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
        bzero(buffer, 256);

	string req_str;
	switch(reqType)
	{
		case 1:
			req_str = "GET "+path+" HTTP/1."+httpVer+"\r\nHost: "+host+"\r\nConnection: keep-alive\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n\r\n";
			break;
		case 2:
			req_str = "HEAD "+path+" HTTP/1."+httpVer+"\r\nHost: "+host+"\r\n\r\n";
			cout << "req_str is: " << req_str << endl;
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
        bzero(buffer, 256);


        while (read(sockfd, buffer, 255) != 0)
        {
                fputs(buffer, stdout);
                bzero(buffer, 256);
        }

	pthread_exit(ret);
}

