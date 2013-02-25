/*
 * File:   SocketServer.hpp
 * Author: jvillegas
 *
 * Created on February 20, 2013
 */

#ifndef SOCKETSERVER_HPP
#define SOCKETSERVER_HPP

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <string>
#include <cstring>
#include <iostream>
#include "netutils.hpp"
#include "SocketServerException.hpp"

using namespace std;
using namespace netutils;

class SocketServer
{
	
	private:
		static const int LISTEN_BACKLOG = 20;
		int listenfd, connfd, status; // socket descriptors
		pthread_t threadid;   // thread id
		socklen_t sin_size;
		sockaddr_storage remote_addr;
		char s[INET6_ADDRSTRLEN];
		struct addrinfo hints, *servinfo, *res, *p; // address info struct
		string bind_iface, bind_port;		
		void listenSocket();
		void *get_in_addr(struct sockaddr *);
	public:
		// ai_family, ai_socktype, ai_protocol
		SocketServer(string, string, int, int);
		void waitConnect();



};

SocketServer::SocketServer(string host = "localhost", 
		string port = "7777", 
		int ai_family = AF_UNSPEC, 
		int ai_socktype = SOCK_STREAM	)		
{	
	memset(&servinfo, 0, sizeof servinfo);
	bind_iface = getIpAddr(host);
	bind_port = port;
	resolveIpAddr(host);
	cout << bind_iface << ":" << bind_port << endl;
	hints.ai_family = ai_family;
	hints.ai_socktype = ai_socktype;
	hints.ai_flags = AI_PASSIVE;
	
	
	listenSocket();
//	waitConnect();
	
}


void SocketServer::listenSocket()
{
	
	
	if((status = getaddrinfo(bind_iface.c_str(), bind_port.c_str(), &hints, &servinfo)) != 0)
	{
		throw new SocketServerException( (char*) gai_strerror( status ));
		exit(1);
	}
	p = servinfo;
	cout << p->ai_family << " " << p->ai_socktype << " " << p->ai_protocol << endl;	
	listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);	
	
	if(listenfd == -1)
	{
		throw new SocketServerException(strerror(errno));
	}
	bind(listenfd, res->ai_addr, res->ai_addrlen);
	listen(listenfd, LISTEN_BACKLOG);

}

void SocketServer::waitConnect()
{
	cout << "Waiting for connection...";
	while(1)
	{
		sin_size = sizeof remote_addr;
		connfd = accept ( listenfd, (struct sockaddr *) &remote_addr, &sin_size);
		if (connfd == -1)
		{
			throw new SocketServerException("incoming connection could not be established");			
			continue;
		}
		
		// convert ip to human readable form
		inet_ntop ( remote_addr.ss_family,
			get_in_addr( (struct sockaddr *) &remote_addr),
			s, sizeof s);
		
		cout << "server: incoming connection from " << s;
		
		if ( !fork() )
		{
			if ( send ( connfd, "Hello world", 13, 0 ) == -1)
				perror ("send");
			close ( connfd );
			exit(0);
		}
		
		close(connfd);
	}
		
			
}	
		
// get sockaddr, IPv4 or IPv6:
void *SocketServer::get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
		
#endif
