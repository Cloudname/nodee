// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef SCRIPT_H
#define SCRIPT_H

#include "process.h"

#include <list>
#include <string>

using namespace std;


class Script
    : public Process
{
public:
    Script( string, list<string>, int, int );
    ~Script() {}
    void start();

private:
    string p;
    list<string> a;
    int u;
    int g;
};

#endif
