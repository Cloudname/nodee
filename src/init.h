// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef INIT_H
#define INIT_H

#include "global.h"


class Init
{
public:
    Init();
    ~Init();

    void operator()() { start(); } // what boost::thread wants

    void start();

private:
    std::list<class Process> l;
};

#endif
