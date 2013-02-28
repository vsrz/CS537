#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
using namespace std;

int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, clilen, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	if (argc < 3)
	{
		cout << "Usage: %s hostname port" << endl;
		return 0;
	}

	portno = atoi(argv[2]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		cout << "Error opening socket" << endl;
		return 0;
	}

	server = gethostbyname(argv[1]);

	if (server == NULL)
	{
		cout << "Error: no such host." << endl;
		return 0;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		cout << "Error connecting" << endl;
		return 0;
	}
	bzero(buffer, 256);
	cout << "Enter request: " << endl;
//	fgets(buffer,255,stdin);
	string host = argv[1];
//	string get_str = "GET / HTTP/1.1\nHost: "+host+"\nConnection: keep-alive\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\n\n";
	string get_str = "GET / HTTP/1.0\n\n";
//	char get = ("GET / HTTP/1.1\nHost: %d\n\n", argv[1]);
	
	
	char *get = new char[get_str.size() + 1];
	get[get_str.size()] = 0;
	memcpy(get, get_str.c_str(), get_str.size());

	n = write(sockfd, get, strlen(get));
	if (n < 0)
	{
		cout << "Error writing to socket" << endl;
		return 0;
	}
	bzero(buffer, 256);
	

	/*
	n = read(sockfd, buffer, 255);
	if (n < 0)
	{
		cout << "Error reading from socket" << endl;
		return 0;
	}
	printf("%s\n",buffer);
	close(sockfd);
	*/

	while (read(sockfd, buffer, 255) != 0)
	{
		fputs(buffer, stdout);
		bzero(buffer, 256);
	}

	/*
	if (read(sockfd, buffer, 255) == 0)
	{
		cout << "Error reading from socket\n";
		return 0;
	}
	fputs(buffer, stdout);
	*/
	
	cout << "\n";
	return 0;
}

