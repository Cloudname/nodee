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

    void operator()();

    void start();
    void check();

    std::list<Process *>& processes();

    void manage( Process * p );

    Process * find( int ) const;

private:
    std::list<Process *> l;
};

#endif
