# Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

CFLAGS=-O3 -W -Wall -Werror

all: dropprivileges nodee

dropprivileges: dropprivileges.c
	clang -o dropprivileges $(CFLAGS) dropprivileges.c

OBJECTS=chorekeeper.o httplistener.o httpserver.o init.o \
	process.o serverspec.o

.cpp.o:
	g++ -g -c -o $@ -O0 $<

ifeq ($(shell ./platform.sh), oneiric)
BOOSTLIBS=-lboost_thread -lboost_filesystem -lboost_system \
		-lboost_program_options
endif
ifeq ($(shell ./platform.sh), lucid)
BOOSTLIBS=-lboost_thread -lboost_filesystem -lboost_program_options
endif

nodee: ${OBJECTS} nodee.o Makefile
	g++ -g -o nodee -pthread ${OBJECTS} ${BOOSTLIBS}

clean:
	-rm nodee nodeetest dropprivileges *.o

nodeetest: ${OBJECTS} test.o Makefile
	g++ -g -o nodeetest -pthread ${OBJECTS} test.o ${BOOSTLIBS}
