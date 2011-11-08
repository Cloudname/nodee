// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "serverspec.h"

#include <boost/property_tree/json_parser.hpp>


/*! \class ServerSpec serverspec.h

    The ServerSpec class contains the complete specification for
    running a server using Nodee.

    Users generally submit this in JSON, this class parses it and
    makes it available to the user of this class.
*/


/*! This constructor is private; the only public way to make a
    ServerSpec is to call parseJson().
*/

ServerSpec::ServerSpec()
    : p( 0 ), eim( 0 ), epm( 0 ), v( 0 )
{
    // nothing more needed
}


/*! Parses a json specification and sets up an object. If the parsing
    failed, the object's coordinate() will be a null string afterwards.
*/

ServerSpec ServerSpec::parseJson( const String & specification )
{
    using boost::property_tree::json_parser;

    ServerSpec s;
 
    ptree pt;
    try {
	read_json( istringstream( specification ), pt );
    } catch ( json_parser_error ) {
	return s;
    }

    try {
	s.c = pt.get<std::string>( "tarball" );
	s.p = pt.get<int>( "port", 0 );
	s.eim = pt.get<int>( "typicalram", 0 );
	s.epm = pt.get<int>( "peakram", 0 );
	s.v = pt.get<int>( "value", 0 );
    } catch ( ptree_bad_data ) {
	s.c.clear();
    }

    return s;
}


/*! Returns the coordinate set by parseJson(), typically a string like
    http://www.example.com/mumble/stumble.tar.gz

    If this is an empty string, then parseJson() didn't like its input
    at all.
*/

string ServerSpec::coordinate() const
{
    return c;
}


/*! Returns the port specified in JSON, or 0 if none was specified. */

int ServerSpec::port() const
{
    return p;
}


/*! Returns the expected initial memory consumption of the server in
    kilobytes, or 0 if none was specified.
*/

int ServerSpec::expectedInitialMemory() const
{
    return eim;
}


/*! Returns the expected peak memory consumption of the server in
    kilobytes, or 0 if none was specified.
*/

int ServerSpec::expectedPeakMemory() const
{
    return epm;
}


/*! Returns the server's value, or 0 if none was specified. A bigger
    value means that the server is more valuable. If the host runs out
    of CPU/RAM, then something will be killed, ideally something that
    uses a lot of that resource and has low value.
*/

int ServerSpec::value() const
{
    return v;
}
