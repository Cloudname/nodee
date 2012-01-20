// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef SERVERSPEC_H
#define SERVERSPEC_H

#include <string>
#include <boost/property_tree/ptree.hpp>



using namespace std;


class ServerSpec
{
public:
    ServerSpec();

    static ServerSpec parseJson( const string & );
    string json() const;

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

    bool valid() const;

    void setError( const string & );
    string error() const;

private:
    ::boost::property_tree::ptree pt;
    string e;
};


#endif
