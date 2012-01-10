// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#ifndef UID_H
#define UID_H

#include <set>

int chooseFreeUid();
int chooseFreeGid();

// only for testing:
std::set<int> inPasswd( bool gid, const char * filename );
std::set<int> inGroup();
std::set<int> inProc( bool gid, const char * filename );

#endif
