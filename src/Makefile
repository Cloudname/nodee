# Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

COMPILER=g++
CFLAGS=-O3 -W -Wall -Werror

all: dropprivileges nodee nodeetest

dropprivileges: dropprivileges.c
	${COMPILER} -o dropprivileges $(CFLAGS) dropprivileges.c

OBJECTS=chorekeeper.o httplistener.o httpserver.o init.o \
	process.o serverspec.o service.o uid.o script.o conf.o

.cpp.o: Makefile
	${COMPILER} -g -c -o $@ -O0 $<

ifeq ($(shell ./platform.sh), oneiric)
BOOSTLIBS=-lboost_thread -lboost_filesystem -lboost_system \
		-lboost_program_options
endif
ifeq ($(shell ./platform.sh), lucid)
BOOSTLIBS=-lboost_thread -lboost_filesystem -lboost_program_options
endif

nodee: ${OBJECTS} nodee.o Makefile
	${COMPILER} -g -o nodee -pthread ${OBJECTS} nodee.o ${BOOSTLIBS}

clean:
	-rm nodee nodeetest dropprivileges *.o

nodeetest: ${OBJECTS} test.o Makefile
	${COMPILER} -g -o nodeetest -pthread ${OBJECTS} test.o ${BOOSTLIBS}
