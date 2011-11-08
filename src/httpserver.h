// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "global.h"


class HttpServer
    : public Server
{
public:
    HttpServer( int );
    
    void operator()() { start(); }

    void start();

    void readRequest();
    void parseRequest();
    void readBody();

    string body() const;
    int contentLength() const;
    Operation operation() const;
    string path() const;

    void respond();

private:
    string p;
    string h;
    string b;
    Operation o;
    int cl;
};


#endif
