// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef SERVERSPEC_H
#define SERVERSPEC_H

#include <string>


using namespace std;


class ServerSpec
{
public:
    ServerSpec();

    static ServerSpec parseJson( const string & );

    string coordinate() const;
    int port() const;
    int expectedTypicalMemory() const;
    int expectedPeakMemory() const;
    int value() const;

    bool valid() const { return !c.empty(); }

private:
    string c;
    int p;
    int eim;
    int epm;
    int v;
};


#endif
