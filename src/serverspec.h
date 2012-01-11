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
    string artifact() const;
    string artifactUrl() const;
    string artifactFilename() const;
    int port() const;
    int expectedTypicalMemory() const;
    int expectedPeakMemory() const;
    int value() const;

    string startupScript() const;
    string shutdownScript() const;

    bool valid() const { return !c.empty(); }

    void setError( const string & );
    string error() const;

private:
    string e;
    string c;
    int p;
    int eim;
    int epm;
    int v;
    string su;
    string sd;
    string a;
};


#endif
