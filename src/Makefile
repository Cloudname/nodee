# Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

CFLAGS=-O3 -W -Wall -Werror

all: dropprivileges nodee

dropprivileges: dropprivileges.c
	clang -o dropprivileges $(CFLAGS) dropprivileges.c

SOURCE=chorekeeper.cpp httplistener.cpp httpserver.cpp init.cpp \
	nodee.cpp process.cpp serverspec.cpp

nodee: ${SOURCE}
	g++ -o nodee -pthread ${SOURCE}
