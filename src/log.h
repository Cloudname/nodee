// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#if !defined LOG_H
#define LOG_H

#include <iostream>
#include <fstream>


extern std::ofstream info;
extern std::ofstream debug;

class Log {
public:
    static void setupStderr();
};

#endif
