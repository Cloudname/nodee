// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef PORT_H
#define PORT_H

#include <set>

using namespace std;


class Port
{
public:
    static set<int> busy( const char * );
    static int assignFree( const std::set<int> & = set<int>() );
};

#endif
