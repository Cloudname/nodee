// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef INIT_H
#define INIT_H

#include "process.h"
#include <list>


class Init
{
public:
    Init();
    ~Init();

    void operator()() { start(); } // what boost::thread wants

    void start();
    void check();

    std::list<Process>::iterator processes();

private:
    std::list<Process> l;
};

#endif
