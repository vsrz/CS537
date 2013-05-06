#ifndef PQUEUE_H
#define PQUEUE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "node.h"
using namespace std;

class pQueue
{
private:
    vector<node> queue;
    struct myclass
    {
        bool operator() (node i, node j)
        {
            int a = i.getDistance();
            int b = j.getDistance();
            if (a == -1)
            {
                return true;
            }
            else if (b == -1)
            {
                return false;
            }
            else
            {
                return (b < a);
            }
        }
    } myobject;
    void qSort()
    {
        sort(queue.begin(), queue.end(), myobject);
    }

public:
    pQueue() {};
    node deleteMin() //pop the top
    {
        node min = queue.back();
        queue.pop_back();
        return min;
    }
    void insert(node newNode)
    {
        queue.push_back(newNode);
    }
    void initialize(int size) //create all the nodes
    {
        for (int i = 0; i < size; i++)
        {
            queue.push_back(node(i));
        }
    }
    void setSource(int source)
    {
        for (int i = 0; i < queue.size(); i++)
        {
            if (queue[i].getName() == source)
            {
                queue[i].setDistance(0);
            }
        }
        qSort();
    }
    void updateNode(int hop, int parent, int dist)
    {
        for (int i = 0; i < queue.size(); i++)
        {
            if (queue[i].getName() == hop)
            {
                queue[i].setParent(parent);
                queue[i].setDistance(dist);
            }
        }
        qSort();
    }
    int getNodeDist(int name)
    {
        int nodeDist = -1;
        for (int i = 0; i < queue.size(); i++)
        {
            if (queue[i].getName() == name)
            {
                nodeDist = queue[i].getDistance();
            }
        }
        return nodeDist;
    }
    void print()
    {
        for (int i = 0; i < queue.size(); i++)
        {
            cout << queue[i].getName() << "d=" << queue[i].getDistance() << " | |";
        }
        cout << endl;
    }
};

#endif //PQUEUE_H