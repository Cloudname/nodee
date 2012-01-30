// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef PROCESS_H
#define PROCESS_H

#include "serverspec.h"


class Process
{
public:
    Process();
    Process( const Process & );
    Process( int, int );
    virtual ~Process() {}

    int pid() const { return p; }
    bool valid() const { return p > 0; }

    void fork();
    virtual void start();
    virtual void handleExit( int, int );

    void fakefork( int fakepid );

    void stop();

    int value() const { return s.value(); }
    int expectedPeakMemory() const { return s.expectedPeakMemory(); }
    int expectedTypicalMemory() const { return s.expectedTypicalMemory(); }

    void setCurrentRss( int );
    int currentRss() const;
    void setPageFaults( int );
    int recentPageFaults() const;

    bool operator==( const Process & other ) { return p == other.p; }
    void operator=( const Process & other );

    static void launch( const ServerSpec & what, class Init & );

    int uid() const;
    int gid() const;
    void assignUidGid();

    void setRoot( const string & );
    string root() const;

private:
    int p;
    int mp;
    ServerSpec s;
    int faults;
    int prevFaults;
    int rss;
    int u;
    int g;
    Process * next;

    int starts;
    time_t waitUntil;
};


#endif
