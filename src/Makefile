# Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

COMPILER=g++
CFLAGS=-O3 -W -Wall -Werror

all: dropprivileges nodee nodeetest

dropprivileges: dropprivileges.c
	${COMPILER} -o dropprivileges $(CFLAGS) dropprivileges.c

OBJECTS=chorekeeper.o httplistener.o httpserver.o init.o \
	process.o serverspec.o service.o uid.o conf.o \
	hoststatus.o port.o artifact.o zkclient.o

ifeq ($(shell ./platform.sh), oneiric)
BOOSTLIBS=-lboost_thread -lboost_filesystem -lboost_system \
		-lboost_program_options -lzookeeper_mt
ZKINCLUDE=-I/usr/include/c-client-src
endif
ifeq ($(shell ./platform.sh), lucid)
BOOSTLIBS=-lboost_thread -lboost_filesystem -lboost_program_options \
		-lzookeeper_mt
ZKINCLUDE=-I/usr/include/zookeeper
endif

.cpp.o: Makefile
	${COMPILER} ${ZKINCLUDE} -g -c -o $@ -O0 $<

nodee: ${OBJECTS} nodee.o Makefile
	${COMPILER} -g -o nodee -pthread ${OBJECTS} nodee.o ${BOOSTLIBS} 

clean:
	-rm nodee nodeetest dropprivileges *.o

nodeetest: ${OBJECTS} test.o Makefile
	${COMPILER} -g -o nodeetest -pthread ${OBJECTS} test.o ${BOOSTLIBS}
