// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef CHOREKEEPER_H
#define CHOREKEEPER_H

#include "process.h"
#include "init.h"

#include <boost/lexical_cast.hpp>


struct RunningProcess {
    RunningProcess(): pid( 0 ), ppid( 0 ), rss( 0 ), majflt( 0 ) {}
    int pid;
    int ppid;
    int rss;
    int majflt;
};


class ChoreKeeper
{
public:
    ChoreKeeper( Init & );
    virtual ~ChoreKeeper();

    bool valid() const;

    void start();

    void detectThrashing();
    bool isThrashing() const;
    static bool oneBitOfThrashing( int, int, int );

    void scanProcesses( const char *, int );

    Process furthestOverPeak() const;
    Process furthestOverExpected() const;
    Process leastValuable() const;
    Process thrashingMost() const;
    Process biggest() const;

    void readProcVmstat( const char *, int &, int &, int & );

    RunningProcess parseProcStat( string line )
	throw ( boost::bad_lexical_cast );

private:
    bool thrashing[8];
    Init & init;
};




#endif
