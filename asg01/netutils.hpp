/*
 * File:   netutils.hpp
 * Author: jvillegas
 *
 * Created on February 16, 2013, 8:07 AM
 */

#ifndef NETUTILS_HPP
#define	NETUTILS_HPP


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <string>
#include <iostream>


namespace netutils {

    std::string getIpAddr(std::string hostname);
    void resolveIpAddr(std::string hostname);
}


#endif	/* NETUTILS_HPP */

