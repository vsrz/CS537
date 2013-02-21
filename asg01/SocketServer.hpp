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
		int listenfd, connfd; // socket descriptors
		pthread_t threadid;   // thread id
		struct addrinfo servinfo, *res; // address info struct
		string bind_iface, bind_port;


		void bindSocket();
		void getSocket();
	public:
		// ai_family, ai_socktype, ai_protocol
		SocketServer(string, string, int, int, int);


};

SocketServer::SocketServer(string host = "127.0.0.1", 
		string port = "7777", 
		int ai_family = AF_UNSPEC, 
		int ai_socktype = SOCK_STREAM, 
		int ai_protocol = 0 )		
{	
	memset(&servinfo, 0, sizeof servinfo);
	bind_iface = getIpAddr(host);
	bind_port = port;
	servinfo.ai_family = ai_family;
	servinfo.ai_socktype = ai_socktype;
	servinfo.ai_protocol = ai_protocol;
	servinfo.ai_flags = AI_PASSIVE;
	getaddrinfo(bind_iface.c_str(), port.c_str(), &servinfo, &res);
	getSocket();
	bindSocket();
}

void SocketServer::getSocket() {

	listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(listenfd == -1)
	{
		throw new SocketServerException(strerror(errno));
	}
}

void SocketServer::bindSocket() {
	bind(listenfd, res->ai_addr, res->ai_addrlen);

}

#endif
