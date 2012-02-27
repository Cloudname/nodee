// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "service.h"

#include "serverspec.h"
#include "process.h"

#include <stdio.h>

#include <algorithm>

#include <list>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

using boost::property_tree::ptree;

// for some reason, using the type directly instead of via the hack
// typedef bothers the compiler. I see nothing wrong. the expression
// works as a global, works in a global function, and worked in
// HttpServer::respond(), it just didn't work here. I have no
// idea. but right now I don't want to bother about it, I want to run
// on aewc-cn-01.
typedef list<Process *> hack;

/*! \class Service service.h

    Service is a tidiness class, a container for independent
    service-related functions so that they don't need to be global.

    At present list() is the only function. It is possible that
    functions to install and uninstall services may return, as well as
    perhaps a function to provide detailed information for monitoring
    purposes.
*/

/*! Returns a JSON foo describing the processes managed by \a init. */

std::string Service::list( Init & init )
{
    ostringstream os;

    ptree pt;

    hack & pl = init.processes(); // compiler protest at this line
    hack::iterator m( pl.begin() );


    while ( m != pl.end() ) {
	string prefix = "services." +
			boost::lexical_cast<string>( (*m)->pid() );
	try {
	    pt.put( prefix + ".coordinate", (*m)->spec().coordinate() );
	} catch ( ... ) {
	    // if no coordinate is specified, we just don't emit any
	}
	try {
	    pt.put( prefix + ".port", (*m)->spec().port() );
	} catch ( ... ) {
	    // ditto port
	}
	try {
	    pt.put( prefix + ".artifact", (*m)->spec().artifact() );
	} catch ( ... ) {
	    // not emitting artifact is a bit shady, though
	}
	pt.put( prefix + ".value", (*m)->spec().value() );
	pt.put( prefix + ".rss", (*m)->currentRss() );
	pt.put( prefix + ".recentfaults", (*m)->recentPageFaults() );
	++m;
    }

    write_json( os, pt );

    return os.str();
}
