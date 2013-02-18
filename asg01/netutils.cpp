/* 
 * File:   netutils.hpp
 * Author: jvillegas
 *
 * Created on February 16, 2013, 8:07 AM
 */

#include "netutils.hpp"

namespace netutils {

    /// Returns a single IP address based on a hostname
    std::string getIpAddr(std::string hostname) {
        struct addrinfo hints, *res;
        char ipstr[INET6_ADDRSTRLEN];    
        char* err;
        int status;
        void* addr;


        status = getaddrinfo(hostname.c_str(), NULL, &hints, &res);
        if(status != 0) {        
            return "";
        }

        if(res->ai_family == AF_INET) {
            struct sockaddr_in *ip = (struct sockaddr_in*)res->ai_addr;
            addr = &(ip->sin_addr);
        } else {
            struct sockaddr_in6 *ip = (struct sockaddr_in6*)res->ai_addr;
            addr = &(ip->sin6_addr);
        }

        inet_ntop(res->ai_family, addr, ipstr, sizeof ipstr);
        return ipstr;


    }

}
