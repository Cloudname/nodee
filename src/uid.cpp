// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <set>

#include <fstream>

#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "uid.h"

using namespace std;
using namespace boost::filesystem;

/*! \nodoc */


/* ubuntu 10.04 includes boost 1.40, which has boost::filesystem API
   2. 11.10 includes API 3. rather than demand and upgraded or
   downgraded boost, I tried to write code that works with both. this
   wrapper is needed to get a filename from an iterator.
*/

static string filename( const boost::filesystem::directory_iterator & i )
{
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 3
    return i->path().native();
#else
    return i->string();
#endif
}


set<int> inPasswd( bool gid, const char * filename )
{
    set<int> passwd;

    ifstream p( filename );
    boost::char_separator<char> x( ":" );
    while ( p ) {
	string line;
	getline( p, line );
	if ( !line.empty() ) {
	    boost::tokenizer<boost::char_separator<char> > t( line, x );
	    boost::tokenizer<boost::char_separator<char> >::iterator i
		= t.begin();
	    boost::tokenizer<boost::char_separator<char> >::iterator e
		= t.end();
	    // i points to the user name
	    ++i; // now i points to the password
	    ++i; // now i points to the UID
	    if ( gid )
		++i; // i now points to the GID
	    try {
		if ( i != e )
		    passwd.insert( boost::lexical_cast<int>( *i ) );
	    } catch ( boost::bad_lexical_cast ) {
		// nothing need be done
	    }
	}
    }
    return passwd;
}


set<int> inGroup() {
    return inPasswd( false, "/etc/group" ); // oh NO how evil. this is perl.
}


set<int> inProc( bool gid, const char * proc ) {
    path p ( proc );
    set<int> r;

    typedef boost::tokenizer<boost::char_separator<char> > bt;

    try {
	boost::char_separator<char> sep( ":" );
	boost::char_separator<char> tab( "\t" );
	directory_iterator i = directory_iterator( p );
	while ( i != directory_iterator() ) {
	    if ( *filename( i ).rbegin() <= '9' && is_directory( p ) ) {
		string x = filename( i );
		x.append( "/status" );
		ifstream status( x.data() );
		while ( status ) {
		    string line;
		    getline( status, line );
		    if ( !line.empty() ) {
			bt t( line, sep );
			bt::iterator i = t.begin();

			string n = *i;
			++i;

			if ( i != t.end() &&
			     ( ( gid && n == "Gid" ) ||
			       ( !gid && n == "Uid" ) ) ) {
			    // *i is now a tab-separated set of ints:
			    // uid, euid, fsuid and whatever. we want
			    // to avoid all of them, so we can just
			    // slurp them in and ignore all the
			    // details.
			    bt v( *i, tab );
			    bt::iterator ti = v.begin();
			    while ( ti != v.end() ) {
				try {
				    // I think this is a little too
				    // deeply indented, really.
				    r.insert( boost::lexical_cast<int>( *ti ) );
				} catch (  boost::bad_lexical_cast ) {
				    // if parseProcStat throws, then
				    // the kernel is behaving oddly.
				    // we ignore that "number" and
				    // look at the rest
				}
				++ti;
			    }
			}
		    }
		}
	    }
	    ++i;
	}
    } catch (const filesystem_error& ex) {
	// hm. really strange. when might this happen? races?
    }
    return r;
}


// return the smallest integer 2000 <= x <= 60000 that's not in use
// either in /etc/passwd or for a running process. 60000 means
// failure. in that case we're managing 58000 services, which seems
// high.

int chooseFreeUid() {
    set<int> passwd = inPasswd( false, "/etc/passwd" );
    set<int> running = inProc( false, "/proc" );
    int i = 2000;
    while ( i < 60000 &&
	    ( passwd.find( i ) != passwd.end() ||
	      running.find( i ) != running.end() ) )
	i++;
    return i;
}

// as for chooseFreeUid(), except a gid, and anything mentioned in
// /etc/group is used, too.

int chooseFreeGid() {
    set<int> group = inGroup();
    set<int> passwd = inPasswd( true, "/etc/passwd" );
    set<int> running = inProc( true, "/proc" );
    int i = 2000;
    while ( i < 60000 &&
	    ( group.find( i ) != group.end() ||
	      passwd.find( i ) != passwd.end() ||
	      running.find( i ) != running.end() ) )
	i++;
    return i;
}
