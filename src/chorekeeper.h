// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef CHOREKEEPER_H
#define CHOREKEEPER_H

#include "process.h"
#include "init.h"



class ChoreKeeper
{
public:
    ChoreKeeper( Init & );
    virtual ~ChoreKeeper();

    void start();

    void detectThrashing();
    bool isThrashing() const;
    static bool oneBitOfThrashing( int, int, int );

    void scanProcesses();

    Process furthestOverPeak() const;
    Process furthestOverExpected() const;
    Process leastValuable() const;
    Process thrashingMost() const;
    Process biggest() const;


private:
    void readProcVmstat( const char *, int &, int &, int & );
	
private:
    bool thrashing[8];
    Init & init;
};

#endif
