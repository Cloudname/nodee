// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef HOSTSTATUS_H
#define HOSTSTATUS_H

#include <string>


class HostStatus
{
public:
    HostStatus();
    operator std::string() const;

    static int cores( const char * );

    static int readProcUptime( const char * );

    static void readProcMeminfo( const char *, int &, int & );

private:
    std::string j;
};

#endif
