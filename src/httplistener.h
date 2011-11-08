// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef HTTPLISTENER_H
#define HTTPLISTENER_H

#include "global.h"


Class HttpListener
    : public Server
{
public:
    HttpListener();
    ~HttpListener();

    void operator()() { start(); } // what boost::thread wants

    void start();

private:
    // nothing
}

#endif
