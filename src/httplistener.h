// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef HTTPLISTENER_H
#define HTTPLISTENER_H


class HttpListener
{
public:
    enum Family { V4, V6 };

    HttpListener( enum Family, int port );

    void operator()() { start(); } // what boost::thread wants

    void start();

    bool valid() const;

public:
    volatile int f;
};

#endif
