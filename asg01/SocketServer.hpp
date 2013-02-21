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

class SocketServer
{
	private:
		int listenfd, connfd; // socket descriptors
		pthread_t threadid;   // thread id
		struct sockaddr_in cliaddr, servaddr; // socket struct
		struct addrinfo *res;

		int getSocket(int domain, int type, int protocol);
	public:
		// ai_family, ai_socktype, ai_protocol
		SocketServer(int, int, int);


};

SocketServer::SocketServer(int ai_family = AF_INET, int ai_socktype = SOCK_STREAM, int ai_protocol = 0) {
	getSocket(ai_family, ai_socktype, ai_protocol);
}

int SocketServer::getSocket(int domain, int type, int protocol) {

	listenfd = socket(domain, type, protocol);
	if(listenfd == -1)
	{
		throw new SocketServerException(strerror(errno));
	}
}

#endif
