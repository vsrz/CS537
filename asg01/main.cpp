/*
 * File:   main.cpp
 * Author: jvillegas
 *
 * Created on February 16, 2013, 7:09 AM
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include "netutils.hpp"
#include "SocketServer.hpp"

using namespace std;
using namespace netutils;


int main(int argc, char** argv) {
    string site;
    string res;
/*
    if(argc != 2) {
        cout << "Usage: " << argv[0] << " <ip or hostname>" << endl;
        return 1;
    }

    resolveIpAddr(argv[1]);
*/
    try
    {
		SocketServer s(131,131,131);		
    }
    catch (SocketServerException e)
    {
		cout << e << endl;
    }


    
    return 0;

}

