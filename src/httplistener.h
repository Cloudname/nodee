// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef HTTPLISTENER_H
#define HTTPLISTENER_H

#include "init.h"


class HttpListener
{
public:
    enum Family { V4, V6 };

    HttpListener( Family f, int port, Init & );

    void operator()();

    void start();

    bool valid() const;

public:
    volatile int f;
    Init & init;
};

#endif
