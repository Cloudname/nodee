// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef CONF_H
#define CONF_H

#include <map>
#include <string>

using namespace std;


class Conf
{
public:

    // boost::po wants to write to variables and I didn't feel like
    // writing setters just for blah, so I made these public.
    static map<string,string> depots;
    static string scriptdir;
    static string basedir;
    static string workdir;
    static string artefactdir;
    static string zk;

    static string url( const string &, const string & );
    static string filename( const string & );
};


#endif
