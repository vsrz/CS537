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

using namespace std;
using namespace netutils;

int main(int argc, char** argv) {
    string site = argv[1];
    string res = getIpAddr(site);
    if(res == "") {
        cout << "Unable to resolve " << site << "\n";
        return 0;
    }
    cout << argv[1] << " has address " << getIpAddr(site) << "\n";
    return 0;
        
}

