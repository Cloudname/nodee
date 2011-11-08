// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef SERVERSPEC_H
#define SERVERSPEC_H

#include "global.h"


class ServerSpec
{
private:
    ServerSpec();

public:
    static ServerSpec parseJson( const String & );

    string coordinate() const;
    int port() const;
    int expectedInitialMemory() const;
    int expectedPeakMemory() const;
    int value() const;

    bool valid() const { return !s.isEmpty(); }

private:
    string c;
    int p;
    int eim;
    int epm;
    int v;
};


#endif
