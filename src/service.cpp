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
typedef list<Process> hack;

/*! Returns a JSON foo describing the processes managed by \a init. */

std::string Service::list( Init & init )
{
    ostringstream os;

    ptree pt;

    hack & pl = init.processes(); // compiler protest at this line
    hack::iterator m( pl.begin() );

    string prefix = "services." + boost::lexical_cast<string>( m->pid() );

    while ( m != pl.end() ) {
	pt.put( prefix + ".value", m->value() );
	pt.put( prefix + ".rss", m->currentRss() );
	pt.put( prefix + ".recentfaults", m->recentPageFaults() );
	++m;
    }

    write_json( os, pt );

    return os.str();
}
