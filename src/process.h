// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef PROCESS_H
#define PROCESS_H


class Process
{
private:
    Process(); // private so it can't be called

public:
    Process();
    Process( const Process & );
    virtual ~Process() {}

    int pid() const;

    void fork();
    void start() = 0;
    virtual void handleExit( int, int );

    int value() const;
    int expectedTypicalMemory() const;
    int expectedPeakMemory() const;

private:
    int p;
};


#endif
