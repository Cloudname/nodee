// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <set>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/tokenizer.hpp>

#include "port.h"

using namespace std;




/*! \class Port port.h

    The Port class contains a couple of static functions to find free
    TCP ports on this system.

    Nodee needs to find free TCP ports, and cannot use the normal
    approach since it has to know the port number much earlier.
    Therefore this class contains some linux-specific /proc parsing to
    supply the numbers we need.

    Now that I write this, I notice a bug. assignFree() should also
    look at all the Process objects and consider any ports assigned
    but not yet used as occupied.
*/

/*! Parses \a filename as though it were linux' /proc/net/tcp and
    returns a set<int> of busy ports.

    Any port used for any purpose is considered busy, even if the use
    we have in mind might be able to coexist with the port's current
    use. The code is simpler that way, there are enough free ports on
    the system, and differentiating between services is easier on the
    poor brain of the sysadmin looking at the system.
*/

set<int> Port::busy( const char * filename )
{
    set<int> taken;

    ifstream p( filename );
    boost::char_separator<char> x( " \t:" );
    while ( p ) {
	string line;
	getline( p, line );
	if ( !line.empty() ) {
	    boost::tokenizer<boost::char_separator<char> > t( line, x );
	    boost::tokenizer<boost::char_separator<char> >::iterator i
		= t.begin();
	    boost::tokenizer<boost::char_separator<char> >::iterator e
		= t.end();
	    string sl, local, localp, remote;
	    // i now points to the line number
	    if ( i != e )
		sl = *i;
	    ++i;
	    if ( i != e )
		local = *i;
	    ++i;
	    if ( i != e )
		localp = *i;
	    ++i;
	    if ( i != e )
		remote = *i;

	    try {
		istringstream tmp( localp );
		int p = 0;
		tmp >> hex >> p;
		taken.insert( p );
	    } catch ( ... ) {
		// will happen for the header line. shouldn't happen
		// for any other line, though.
	    }
	}
    }
    return taken;
}


/*! Returns a free port on this host, avoiding \a avoid (which is an
    empty set by default).

    Ports used for any purpose are assumed to be used; if a process
    opens a TCP connection from TCP6 port 1025, this function will
    never return 1025, even though listening would be legal in most
    cases.

    The range used is 1025-65535; 65535 is returned as a last resort.
*/

int Port::assignFree( const set<int> & avoid )
{
    set<int> taken4;
    set<int> taken6;
    try {
	taken4 = busy( "/proc/net/tcp" );
	taken6 = busy( "/proc/net/tcp6" );
    } catch ( ... ) {
	// we may not have /proc/net. ignore that.
    }
    int p = 1025;
    while ( p < 65535 &&
	    ( taken4.find( p ) != taken4.end() ||
	      taken6.find( p ) != taken6.end() ||
	      avoid.find( p ) != avoid.end() ) )
	p++;
    return p;
}
