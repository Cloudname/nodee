// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef SERVICE_H
#define SERVICE_H

#include <string>

#include "init.h"

using namespace std;


class Service
{
public:
    void foo( Init & );
    static string list( Init & );
};

#endif
