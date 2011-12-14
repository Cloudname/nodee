# Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

CFLAGS=-O3 -W -Wall -Werror

all: dropprivileges nodee

dropprivileges: dropprivileges.c
	clang -o dropprivileges $(CFLAGS) dropprivileges.c

OBJECTS=chorekeeper.o httplistener.o httpserver.o init.o \
	nodee.o process.o serverspec.o

.cpp.o:
	clang -c -o $@ -O0 $<

nodee: ${OBJECTS}
	clang -o nodee -pthread $< -lboost_thread

clean:
	rm nodee dropprivileges *.o
