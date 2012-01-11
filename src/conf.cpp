// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "conf.h"



/*! \class Conf conf.h
  
    The Conf class is a wrapper for some global variables and two
    function to access one variable and return it in the desirable
    format.

    Globals and singletons are highly undesirable, so please don't
    turn the class into more than it is.
*/


map<string,string> Conf::depots;
string Conf::basedir;
string Conf::workdir;
string Conf::artefactdir;
string Conf::zk;


/*! Returns \a artefact turned into an URL we can download. Returns
    an empty string in case of error. Should it perhaps throw instead?
*/

string Conf::url( const string & artefact )
{
    if ( artefact.find( '/' ) <= artefact.size() )
	return "";

    int p1 = artefact.find( ':' );
    int p2 = artefact.rfind( ':' );

    string group = artefact.substr( 0, p1 );
    string thing = artefact.substr( p1 + 1, p2 - p1 - 1 );
    string version = artefact.substr( p2 + 1 );

    if ( group.empty() || thing.empty() || version.empty() )
	return "";

    string depot = depots[group];
    if ( depot.empty() )
	return "";

    // no file type here. fsck that. the file type is indicated within
    // the file anyway.
    return depot + thing + ":" + version;
}



/*! Returns the absolute filename using which \a artefact is stored
    locally.
*/

string Conf::filename( const string & artefact )
{
    if ( artefact.find( '/' ) < artefact.size() )
	return "";

    int p1 = artefact.find( ':' );
    int p2 = artefact.rfind( ':' );

    string group = artefact.substr( 0, p1 );
    string thing = artefact.substr( p1 + 1, p2 - p1 - 1 );
    string version = artefact.substr( p2 + 1 );

    if ( group.empty() || thing.empty() || version.empty() )
	return "";

    string depot = depots[group];
    if ( depot.empty() )
	return "";

    // no file type here. fsck that. the file type is indicated within
    // the file anyway.
    return basedir + "/" + artefactdir + "/" + thing + ":" + version;
}
