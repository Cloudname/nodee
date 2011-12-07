// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>

using namespace std;


class HttpServer
{
public:
    enum Operation { Get, Post, Invalid };

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
    void send( string );

    string httpResponse( int, const string &, const string & );

    void close();

private:
    string p;
    string h;
    string b;
    Operation o;
    int cl;
    int f;
};


#endif
