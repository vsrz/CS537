#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <vector>

#define MAXLINE	32768

using namespace std;

const int backlog = 4;

void fileHandler(string s, int fd)
{
    ifstream instream;
    int length = 256;

    instream.open(s.c_str(), ios::binary);

    char* buffer = new char[length];
    
    if(instream.is_open())
    {  
        while (!instream.eof()) 
        {
            cout << "File open" << endl;
            bzero(buffer,length);
            instream.read(buffer, length);
            write(fd, buffer, length);
        }
    }
    instream.close();

}



void httpGet(vector<string> s, int fd)
{
    string request, path, proto;
    cout << "Received: " << s[0] << endl;
    request = s[0].substr(0,3);
    path = s[0].substr(5,s[0].length() - 14);
    proto = s[0].substr(s[0].length() - 8,s[0].length());

    cout << "Proto is: " << proto << endl;

    string httpOK = proto + " 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n";
    cout << "httpOK is: " << httpOK << endl;
    write(fd, httpOK.c_str(), httpOK.length()+8);
 
    cout << "Path: " << path << endl;
    fileHandler(path,fd); 
}

void *clientHandler(void *arg)
{

    char str[MAXLINE];
    vector<string> cli_buf;
    int  i, n;
    bool conn = true,done;
    char *pch;
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
//	vector<string> split;
	string req(str);
	stringstream ss(req);
	string item;
	while (getline(ss, item, '\r'))
	{
		cli_buf.push_back(item);
	}
//	cout << "Got past spliting!" << endl;
/*
	pch = strtok(str,"\r");
        while (pch != NULL)
	{
		cout << " pch: " << pch << endl;
        	cli_buf.push_back(string(pch));		
		pch = strtok(NULL, "\r\n");
	}
*/

	if (cli_buf[0].substr(0, 3) == "GET")
	{
		httpGet(cli_buf, fd);
	}

	cout << "clibuf: " << cli_buf[0] << endl;

        
    }
    
}

int main(int argc, char *argv[])
{

	int	listenfd, connfd;
        pthread_t tid;
	socklen_t clilen;
	char bind_addr[] = "144.37.1.24";
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
