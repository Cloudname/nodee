// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef ZKCLIENT_H
#define ZKCLIENT_H

#include "process.h"

#include <zookeeper/zookeeper.h> // sigh


class ZkClient: public Process
{
public:
    ZkClient( const std::string & server );

    void operator()() { start(); } // what boost::thread wants

    void start();

private:
    zhandle_t * zh;
    std::string path;
};

#endif
