#ifndef NODE_H
#define NODE_H

#include <iostream>
using namespace std;

class node
{
private:
    int parent;
    int distance;
    int name;
public:
    node(int newName)
    {
        parent = NULL;
        distance = -1;
        name = newName;
    }
    void setParent(int newParent)
    {
        parent = newParent;
    }
    void setDistance(int newDistance)
    {
        distance = newDistance;
    }
    int getDistance()
    {
        return distance;
    }
    int getName()
    {
        return name;
    }
    int getParent()
    {
        return parent;
    }
};

#endif //NODE_H