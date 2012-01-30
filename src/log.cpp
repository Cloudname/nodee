// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "log.h'

ostream info;
ostream debug;



void Log::setupStderr() {
    info = cerr;
    debug = cerr;
}
