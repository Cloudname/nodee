// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "serverspec.h"
#include "conf.h"
#include "port.h"
#include "init.h"




/*! \class ServerSpec serverspec.h

    The ServerSpec class contains the complete specification for
    running a server using Nodee.

    Users generally submit this in JSON, this class parses it and
    makes it available to the user of this class.

    Here's a json example (not very nicely formatted, but json parsers
    don't care and neither do I, so don't bother complaining): {
  "coordinate" : "1.idee-prod.ideeuser.ie",
  "artifact" : "com.telenor:id-server:1.4.2",
  "filename" : "id-server-1.4.2-shaded.jar",
  "md5" : "2c6ca63c97c04c821613f1251643c3bb",
  "options" : {
    "--someoption" : "some value",
    "--anotheroption" : "more config"
  },
  "restart" : {
    "period" : 120,
    "maxrestarts" : 10
  }
}

    Note that you cannot specify any single option twice. -foo 1 --foo
    2 is not possible; nodee will use one of the two.
*/


/*! This constructor is private; the only public way to make a
    ServerSpec is to call parseJson().
*/

ServerSpec::ServerSpec()
{
    // nothing more needed
}


/*! Parses a json \a specification and sets up an object. If the parsing
    failed, the object's coordinate() will be a null string afterwards.

    \a init is needed in order to assign defaults that do not conflict
    with any other Process \a init currently manages.
*/

ServerSpec ServerSpec::parseJson( const string & specification,
				  Init & init )
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
    } catch ( ... ) {
	set<int> used;

	list<Process *> & pl = init.processes();
	list<Process *>::iterator m( pl.begin() );
	while ( m != pl.end() ) {
	    used.insert( (*m)->spec().port() );
	    ++m;
	}
	
	s.pt.put( "port", Port::assignFree( used ) );
    }

    // verify validity and clear the object if necessary
    if ( !s.valid() ) {
	s.pt = ptree();
	return s;
    }

    // options are copied to a separate map, since I don't feel like
    // experimenting to find out how to remove things from ptree.

    ptree::const_assoc_iterator i;

    try {
	ptree options = s.pt.get_child( "options" );
	ptree::const_iterator i = options.begin();
	while ( i != options.end() ) {
	    s.o[i->first] = i->second.data();
	    ++i;
	}
    } catch ( ... ) {
	s.setError( "Error using supplied options" );
    }

    return s;
}


/*! Returns a json object corresponding to this ServerSpec. If this
    object is !valid(), then the return value is "{}".
*/

string ServerSpec::json() const
{
    ostringstream os;
    write_json( os, pt );
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


/*! Returns the minimum number of seconds between restarts of this
    service, which is 0 by default.
*/

int ServerSpec::restartPeriod() const
{
    return pt.get<int>( "restart.period", 0 );
}


/*! Returns the maximum number of times this service may be restarted,
    which is 0 by default (ie. the service may be started, but never
    restarted).
*/

int ServerSpec::maxRestarts() const
{
    return pt.get<int>( "restart.maxrestarts", 0 );
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
    try {
	return pt.get<string>( "artifact" );
    } catch ( ... ) {
	return pt.get<string>( "artefact" );
    }
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

bool ServerSpec::valid()
{
    try {
	(void)pt.get<int>( "expectedpeakram", 0 );
    } catch ( ... ) {
	setError( "Problem regarding expectedpeakram" );
	return false;
    }
    try {
	(void)pt.get<int>( "expectedram", 0 );
    } catch ( ... ) {
	setError( "Problem regarding expectedram" );
	return false;
    }
    int p = 1;
    try {
	p = pt.get<int>( "port" );
    } catch ( ... ) {
	setError( "Problem regarding port" );
	return false;
    }
    if ( p < 1 || p > 65535 ) {
	setError( "Port must be 1-65535" );
	return false;
    }
    try {
	(void)pt.get<int>( "value", 0 );
    } catch ( ... ) {
	setError( "Problem regarding value" );
	return false;
    }
    try {
	(void)pt.get<string>( "artifact" );
    } catch ( ... ) {
	try {
	    (void)pt.get<string>( "artefact" );
	} catch ( ... ) {
	    setError( "Problem regarding artifact" );
	    return false;
	}
    }
    try {
	(void)pt.get<string>( "coordinate" );
    } catch ( ... ) {
	setError( "Problem regarding coordinate" );
	return false;
    }
    try {
	(void)pt.get<string>( "filename" );
    } catch ( ... ) {
	setError( "Problem regarding filename" );
	return false;
    }
    try {
	(void)pt.get<string>( "shutdownscript", "" );
    } catch ( ... ) {
	setError( "Problem regarding " );
	return false;
    }
    try {
	(void)pt.get<string>( "startupscript", "" );
    } catch ( ... ) {
	setError( "Problem regarding " );
	return false;
    }
    try {
	(void)pt.get<string>( "url" );
    } catch ( ... ) {
	setError( "Problem regarding url" );
	return false;
    }

    return true;
}


/*! Writes \a s as startupscript on the ServerSpec. No sanity checking
    is performed.
*/

void ServerSpec::setStartupScript( const string & script,
				   const map<string,string> & options )
{
    pt.put( "startupscript", script );
    o = options;
}


/*! Returns a map containing all the startup options specified; the
    map may be empty.

    This forces all startup command lines to be independent of order,
    and options cannot be repeated. OK.
*/

map<string,string> ServerSpec::startupOptions()
{
    return o;
}


/*! Constructs an deep copy of \a other. */

ServerSpec::ServerSpec( const ServerSpec & other )
    : pt( other.pt ), o( other.o ), e( other.e )
{
}


/*! Returns the MD5 sum specified, or an empty string if none is
    specified.
*/

string ServerSpec::md5() const
{
    return pt.get<string>( "md5", "" );
}
