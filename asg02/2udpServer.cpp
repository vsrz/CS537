#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	int sock, length, n;
	socklen_t fromlen;
	struct sockaddr_in server;
	struct sockaddr_in from;
	char buf[1024];

	if (argc != 3)
	{
		cout << "Usage: udp_server <address> <port> \n";
		return -1;
	}

	sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		cout << "error opening socket\n";
		return -1;
	}
	length = sizeof(server);
	bzero(&server,length);
	server.sin_family=AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port=htons(atoi(argv[2]));
	if (bind(sock,(struct sockaddr *)&server,length)<0)
	{
		cout << "Error binding\n";
	}
	fromlen = sizeof(struct sockaddr_in);
	while (1) 
	{
		n = recvfrom(sock,buf,1024,0,(struct sockaddr *)&from,&fromlen);
		if (n < 0)
		{
			cout << "recvfrom error\n";
			return -1;
		}
		string data(buf);
		cout << "Received a datagram, size: " << data.size() << ", contents: " << buf << endl;
//		write(1,"Received a datagram: ",21);
//		write(1,buf,n);
		n = sendto(sock,"ACK",3, 0,(struct sockaddr *)&from,fromlen);
		if (n  < 0)
		{
			cout << "Error sending\n";
			return -1;
		}
	}
	return 0;
}
