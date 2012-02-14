// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.
#ifndef SERVICE_H
#define SERVICE_H

#include <string>

#include "init.h"

using namespace std;


class Service
{
public:
    static string list( Init & );
};

#endif
