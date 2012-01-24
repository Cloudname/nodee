// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "script.h"

#include <stdlib.h>
#include <sysexits.h>


/*! \class Script script.h

  The Script class represents a state transition script, such as Nodee
  uses to download packages, install them, start and stop them.

  Only used by Launcher.
*/


/*! Constructs a Script for \a path with command-line arguments \a
    argv, and notify \a launcher after the script has been run (or
    failed to run).

    \a path needs to be absolute (/foo/bar/download, not just
    download), and \a argv can have a maximum of 1024 entries. The OS
    or shell may impose a much lower limit.
*/

Script::Script( string path, list<string> argv, int uid, int gid )
    : Process(), p( path ), a( argv ), u( uid ), g( gid )
{
}


/*! Starts the script or gives up. */

void Script::start()
{
    char * tmp[1025];
    int n = 0;
    list<string>::iterator i = a.begin();
    while ( i != a.end() && n < 1024 )
	tmp[n++] = const_cast<char*>((*i).c_str());
    tmp[n] = 0;
    ::execv( p.c_str(), tmp );
    // if execv() fails, we have no recourse. this exits the child,
    // though, not nodee itself.
    ::exit( EX_NOINPUT );
}
