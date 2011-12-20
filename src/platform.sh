#!/bin/sh

if [ -f /etc/lsb-release ] ; then
    . /etc/lsb-release
    echo "$DISTRIB_CODENAME"
else
    uname -s
fi

