// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef PROCESS_H
#define PROCESS_H

#include "serverspec.h"


class Process
{
private:
    Process(); // private so it can't be called

public:
    Process( const Process & );
    virtual ~Process() {}

    int pid() const;

    void fork();
    virtual void start();
    virtual void handleExit( int, int );

    int value() const;
    int expectedTypicalMemory() const;
    int expectedPeakMemory() const;

    bool operator==( const Process & other ) { return p == other.p; }

    static void launch( const ServerSpec & what );

private:
    int p;
    ServerSpec s;
};


#endif
