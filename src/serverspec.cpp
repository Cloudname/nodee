// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "serverspec.h"
#include "conf.h"
#include "port.h"




/*! \class ServerSpec serverspec.h

    The ServerSpec class contains the complete specification for
    running a server using Nodee.

    Users generally submit this in JSON, this class parses it and
    makes it available to the user of this class.


    {
  "coordinate" : "1.idee-prod.ideeuser.ie",
  "artifact" : "com.telenor:id-server:1.4.2",
  "filename" : "id-server-1.4.2-shaded.jar",
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

    // parse
    try {
	istringstream i( specification );
	read_json( i, s.pt );
    } catch ( boost::property_tree::json_parser::json_parser_error e ) {
	s.pt = ptree();
	return s;
    }

    // add default settings. this is too much work, really.
    try {
	int p = s.pt.get<int>( "port" );
    } catch ( boost::property_tree::ptree_bad_data ) {
	// this is a really bad error. we'll leave it in, and valid()
	// will return false.
    } catch ( boost::property_tree::ptree_bad_path ) {
	// the path isn't bad, it's just an unassigned node. bad
	// exception naming there. we make up a port.
	s.pt.put( "port", Port::assignFree() );
    }

    // verify validity and clear the object if necessary
    if ( s.valid() ) {
	s.pt = ptree();
	return s;
    }

    // anything more? no?

    return s;
}


/*! Returns a json object corresponding to this ServerSpec. If this
    object is !valid(), then the return value is "{}".
*/

string ServerSpec::json() const
{
    ostringstream os;
    write_json( os, pt, false );
    return os.str();
}


/*! Returns the coordinate set by parseJson(), typically a string like
    1.foobar.i.example.com.

    Returns an empty string if the object is not valid().
*/

string ServerSpec::coordinate() const
{
    return pt.get<string>( "coordinate" );
}


/*! Returns the port specified in JSON, or the random number picked at
    read time was specified.
*/

int ServerSpec::port() const
{
    return pt.get<int>( "port" );
}


/*! Returns the expected typical memory consumption of the server in
    kilobytes, or 0 if none was specified.
*/

int ServerSpec::expectedTypicalMemory() const
{
    return pt.get<int>( "expectedram", 0 );
}


/*! Returns the expected peak memory consumption of the server in
    kilobytes, or 0 if none was specified.
*/

int ServerSpec::expectedPeakMemory() const
{
    return pt.get<int>( "expectedpeakram", 0 );
}


/*! Returns the server's value, or 0 if none was specified. A bigger
    value means that the server is more valuable. If the host runs out
    of CPU/RAM, then something will be killed, ideally something that
    uses a lot of that resource and has low value.
*/

int ServerSpec::value() const
{
    return pt.get<int>( "value", 0 );
}


/*! Returns the path to the server's startup script, relative to the
    server's own root. Typically this will be something like
    scripts/startup.sh, although it may be anything.
    
    Returns an empty string if none has been set.
*/

string ServerSpec::startupScript() const
{
    return pt.get<string>( "startupscript", "" );
}


/*! Returns the path to the server's shutdowbn script, relative to the
    server's own root. Typically this will be something like
    scripts/shutdown.sh, although it may be anything.
    
    Returns an empty string if none has been set.
*/

string ServerSpec::shutdownScript() const
{
    return pt.get<string>( "shutdownscript", "" );
}


/*! Returns the specified artifact, typically a string such as
 comoyo:nodee:1.0.0.
 */

string ServerSpec::artifact() const
{
    return pt.get<string>( "artifact" );
}


/*! Returns the URL corresponding to this artifact. Always succeeds if
    the ServerSpec is valid().
*/

string ServerSpec::artifactUrl() const
{
    return pt.get<string>( "url" );
}


/*! Returns the URL corresponding to this artifact. Always succeeds if
    the ServerSpec is valid().
*/

string ServerSpec::artifactFilename() const
{
    return pt.get<string>( "filename" );
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


/*! Returns true if the ServerSpec is valid and usable, and false if
    there is any kind of error, e.g. port being a string or artifact
    not being supplied. Throws absolutely no exceptions.
*/

bool ServerSpec::valid() const
{
    try {
	(void)pt.get<int>( "expectedpeakram", 0 );
	(void)pt.get<int>( "expectedram", 0 );
	int p = pt.get<int>( "port" );
	if ( p < 1 || p > 65535 )
	    return false;
	(void)pt.get<int>( "value", 0 );
	(void)pt.get<string>( "artifact" );
	(void)pt.get<string>( "coordinate" );
	(void)pt.get<string>( "filename" );
	(void)pt.get<string>( "shutdownscript", "" );
	(void)pt.get<string>( "startupscript", "" );
	(void)pt.get<string>( "url" );
    } catch ( ... ) {
	return false;
    }
    return true;
}
