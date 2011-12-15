# Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

CFLAGS=-O3 -W -Wall -Werror

all: dropprivileges nodee

dropprivileges: dropprivileges.c
	clang -o dropprivileges $(CFLAGS) dropprivileges.c

OBJECTS=chorekeeper.o httplistener.o httpserver.o init.o \
	nodee.o process.o serverspec.o

.cpp.o:
	g++ -c -o $@ -O0 $<

nodee: ${OBJECTS}
	g++ -o nodee -pthread ${OBJECTS} -lboost_thread -lboost_filesystem

clean:
	rm nodee dropprivileges *.o
