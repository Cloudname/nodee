// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef CHOREKEEPER_H
#define CHOREKEEPER_H


class ChoreKeeper
{
public:
    ChoreKeeper();
    virtual ~ChoreKeeper();

    void start();

    void detectThrashing();
    bool isThrashing() const;
    
    void readMemoryStatus();
    void readCpuStatus();
    void adjustProcesses();

private:
    void readProcFile( const char *, int &, int &, int & );
	
private:
    bool thrashing[8];
    
    
};

#endif
