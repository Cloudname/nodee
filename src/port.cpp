// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <set>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <boost/tokenizer.hpp>

#include "port.h"

using namespace std;




/*!

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


/*! Returns a free port on this host.

    Ports used for any purpose are assumed to be used; if a process
    opens a TCP connection from TCP6 port 1025, this function will
    never return 1025, even though listening would be legal in most
    cases.
    
    The range used is 1025-65535; 65535 is returned as a last resort.
*/

int Port::assignFree()
{
    set<int> taken4 = busy( "/proc/net/tcp" );
    set<int> taken6 = busy( "/proc/net/tcp" );
    int p = 1025;
    while ( p < 65535 && 
	    ( taken4.find( p ) != taken4.end() ||
	      taken6.find( p ) != taken4.end() ) )
	p++;
    return p;
}
