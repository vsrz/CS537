/*
 * File:   SocketServerException.hpp
 * Author: jvillegas
 *
 * Created on February 20, 2013, 7:09 AM
 */

#include <iostream>

using namespace std;

class SocketServerException
{
    char * message;
public:
    SocketServerException(char *msg)
    : message(msg)
    {
    }
    inline friend ostream &operator << (ostream &out, SocketServerException &e)
    {
        return out << "SocketServerException: " << e.message;
    }
};
