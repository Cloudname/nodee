// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "hoststatus.h"

#include <unistd.h>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

using boost::property_tree::ptree;
using namespace std;


/*! \class HostStatus hoststatus.h

  The HostStatus class gathers host status information and hands it
  out in JSON format.

  The point of this is to store nodee information in a zookeeper
  ephemeral node. As a side benefit, we'll also hand it out to anyone
  who asks nicely via HTTP.
*/


/*! Constructs a host status object, gathering all the data. */

HostStatus::HostStatus()
{
    ostringstream os;

    ptree pt;

    char tmp[1025];
    tmp[1024] = 0;
    ::gethostname( tmp, 1024 );

    int total, available;
    readProcMeminfo( "/proc/meminfo", total, available );
    int uptime = readProcUptime( "/proc/uptime" );

    // do I really want that prefix? no? not sure.
    string prefix = string("hosts.") + tmp;
    if ( total )
	pt.put( prefix + ".totalmemory", total );
    if ( available )
	pt.put( prefix + ".available", available );
    if ( uptime )
	pt.put( prefix + ".uptime", uptime );
    pt.put( prefix + ".cores", cores( "/proc/cpuinfo" ) );

    write_json( os, pt );

    j = os.str();
}


/*! Returns the status in json format. */

HostStatus::operator string() const
{
    return j;
}


/*! Parses \a filename as though it were /proc/cpuinfo and returns the
    number of cores.

    /proc/cpuinfo also contains much other information, but I don't
    want to use it. Some we don't want to depend on, some we might
    want to use, but it changes. For instance, my desktop currently
    has "800MHz" cores, which tells me more about power saving than
    about CPU capacity.

*/

int HostStatus::cores( const char * filename )
{
    int r = 1; // we know we have one, since this line runs
    try {
    ifstream cpuinfo( filename );
    boost::char_separator<char> sep( ": " );
    while ( cpuinfo ) {
	string line;
	getline( cpuinfo, line );
	if ( !line.empty() ) {
	    if ( line.substr( 0, 9 ) == "processor" ) {
		boost::tokenizer<boost::char_separator<char> > t( line, sep );
		boost::tokenizer<boost::char_separator<char> >::iterator i
		    = t.begin();
		++i;
		int v = boost::lexical_cast<int>( *i );
		if ( v >= r )
		    r = v + 1;
	    }
	}
    }
    } catch( ... ) {
	// this may fail, in which case we'll contain all exceptions
	// and just return that we have one core (or several, if the
	// failure happened after we found the right number of cores).
    }
    return r;
}


/*! Reads /proc/meminfo, or a different file if \a filename is not
    /proc/meminfo, and stores the amount of total memory in \a total
    and likely available memory in \a available. Both \a total and \a
    available are in kB.

    \a total and \a available are set to 0 if anything fails.
*/

void HostStatus::readProcMeminfo( const char * filename,
				  int & total, int & available )
{
    total = 0;
    available = 0;
    try {
	ifstream meminfo( filename );
	boost::char_separator<char> col( ":" );
	boost::char_separator<char> space( " " );
	while ( meminfo ) {
	    string line;
	    getline( meminfo, line );
	    if ( !line.empty() ) {
		boost::tokenizer<boost::char_separator<char> > t( line, col );
		boost::tokenizer<boost::char_separator<char> >::iterator i
		    = t.begin();
		string n = *i;
		++i;
		boost::tokenizer<boost::char_separator<char> > o( *i, space );
		try {
		    int v = boost::lexical_cast<int>( *o.begin() );
		    if ( n == "MemTotal" )
			total = v;
		    else if ( n == "MemFree" || n == "Inactive" )
			available += v;
		} catch (  boost::bad_lexical_cast ) {
		}
	    }
	}
    } catch ( ... ) {
	// total and available may be zero, but that's okay
    }
}


/*! Reads \a filename, which is assumed to be /proc/uptime (unless a
    different name is better for testing) and returns the host's
    uptime in seconds.

    Needed (only) for deciding which host is oldest and is therefore
    next to be rebooted.
*/

int HostStatus::readProcUptime( const char * filename )
{
    try {
	ifstream uptime( filename );
	string line;
	getline( uptime, line );
	int i = 0;
	while ( i < line.length() &&
		line[i] >= '0' && line[i] <= '9' )
	    i++;
	if ( i )
	    return boost::lexical_cast<int>( line.substr( 0, i ) );
    } catch ( ... ) {
    }
    return 0;
}

