// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#if !defined LOG_H
#define LOG_H

#include <iostream>


extern ostream & info;
extern ostream & debug;

class Log {
public:
    static void setupStderr();
};

#endif
