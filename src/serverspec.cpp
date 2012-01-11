// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "serverspec.h"
#include "conf.h"




/*! \class ServerSpec serverspec.h

    The ServerSpec class contains the complete specification for
    running a server using Nodee.

    Users generally submit this in JSON, this class parses it and
    makes it available to the user of this class.


    {
  "coordinate" : "1.idee-prod.ideeuser.ie",
  "artifact" : "com.telenor:id-server:1.4.2",
  "options" : {
    "--someoption" : "some value",
    "--anotheroption" : "more config"
  },
  "restart" : {
    "period" : 120,
    "maxrestarts" : 10,
    "enabled" : true
  }
}

*/


/*! This constructor is private; the only public way to make a
    ServerSpec is to call parseJson().
*/

ServerSpec::ServerSpec()
    : p( 0 ), eim( 0 ), epm(  0), v( 0 )
{
    // nothing more needed
}


/*! Parses a json specification and sets up an object. If the parsing
    failed, the object's coordinate() will be a null string afterwards.
*/

ServerSpec ServerSpec::parseJson( const string & specification )
{
    using boost::property_tree::ptree;

    ServerSpec s;

    ptree pt;
    try {
	istringstream i( specification );
	read_json( i, pt );
    } catch ( boost::property_tree::json_parser::json_parser_error e ) {
	return s;
    }

    try {
	s.c = pt.get<std::string>( "tarball" );
	s.p = pt.get<int>( "port", 0 );
	s.eim = pt.get<int>( "typicalram", 0 );
	s.epm = pt.get<int>( "peakram", 0 );
	s.v = pt.get<int>( "value", 0 );
	s.su = pt.get<string>( "startupscript", "" );
	s.sd = pt.get<string>( "shutdownscript", "" );
    } catch ( boost::property_tree::ptree_bad_data e ) {
	s.c.clear();
	s.setError( "Parse error" );
	return s;
    }

    if ( s.a.empty() ) {
	s.setError( "No artifact specified" );
	s.c.clear();
    } else if ( Conf::url( s.a ).empty() ) {
	s.setError( "Unable to find a repository for artifact " + s.a );
	s.c.clear();
    } else if ( Conf::filename( s.a ).empty() ) {
	s.setError( "Unable to construct a file name for artifact " + s.a );
	s.c.clear();
    } else if ( s.c.empty() ) {
	s.setError( "No coordinate specified" );
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


/*! Returns the expected typical memory consumption of the server in
    kilobytes, or 0 if none was specified.
*/

int ServerSpec::expectedTypicalMemory() const
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


/*! Returns the path to the server's startup script, relative to the
    server's own root. Typically this will be something like
    scripts/startup.sh, although it may be anything.
    
    Returns an empty string if none has been set.
*/

string ServerSpec::startupScript() const
{
    return su;
}


/*! Returns the path to the server's shutdowbn script, relative to the
    server's own root. Typically this will be something like
    scripts/shutdown.sh, although it may be anything.
    
    Returns an empty string if none has been set.
*/

string ServerSpec::shutdownScript() const
{
    return sd;
}


/*! Returns the specified artifact, typically a string such as
 comoyo:nodee:1.0.0.
 */

string ServerSpec::artifact() const
{
    return a;
}


/*! Returns the URL corresponding to this artifact. Always succeeds if
    the ServerSpec is valid().
*/

string ServerSpec::artifactUrl() const
{
    return Conf::url( a );
}


/*! Returns the URL corresponding to this artifact. Always succeeds if
    the ServerSpec is valid().
*/

string ServerSpec::artifactFilename() const
{
    return Conf::filename( a );
}


/*! Records that \a error occured while creating this ServerSpec. The
    initial value is an empty string.
*/

void ServerSpec::setError( const string & error )
{
    e = error;
}


/*! Returns whatever setError() set, or an empty string if setError()
    has not been called.
*/

string ServerSpec::error() const
{
    return e;
}
