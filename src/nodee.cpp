// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "httplistener.h"
#include "chorekeeper.h"
#include "zkclient.h"
#include "init.h"
#include "conf.h"

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

#include <boost/algorithm/string.hpp>

#if !defined(CONFFILE)
#define CONFFILE "/etc/nodee/config"
#endif


using namespace boost::program_options;


/*! \nodoc */


int main( int argc, char ** argv )
{
    Conf::setDefaults();

    int port;
    vector<string> depots;
    string cf( CONFFILE );

    options_description cli( "Command-line options" );
    cli.add_options()
	( "help", "produce help message" )
	( "config,C", value<string>(&cf)->default_value( CONFFILE ),
	  "specify configuration file" )
	( "version,V", "show easter egg" );

    options_description conf( "Configuration file (and command-line) options" );
    conf.add_options()
	( "port,P", value<int>( &port )->default_value( 40 ),
	  "set nodee TCP port" )
	( "depot", value<vector<string> >( &depots )->composing(),
	  "add artefact depot (e.g. example=http://artefactory.example.com/)" )
	( "dir",
	  value<string>( &Conf::basedir )->default_value( "/usr/local/nodee" ),
	  "specify base directory" )
	( "workdir",
	  value<string>( &Conf::workdir )->default_value( "work" ),
	  "specify work directory, relative to basedir" )
	( "artefactdir",
	  value<string>( &Conf::artefactdir )->default_value( "artefacts" ),
	  "specify where to store artefacts, relative to basedir" )
	( "zookeeper", value<string>( &Conf::zk ),
	  "zookeeper location (e.g. FIXME)" );

    variables_map vm;

    cli.add( conf );
    store( parse_command_line( argc, argv, cli ), vm );

    store( parse_environment( cli, "NODEE_" ), vm );

    ifstream cfs( cf.c_str() );
    store( parse_config_file( cfs, conf, false ), vm );

    notify( vm );

    bool dumpdepots = false;
    if ( depots.size() ) {
	int i = depots.size();
	while ( i ) {
	    --i;

	    std::vector<std::string> tmp;
	    boost::split( tmp, depots[i], boost::is_any_of("=") );
	    if ( tmp.size() != 2 ) {
		cerr << "nodee: Syntax error in artefact depot: "
		     << depots[i] << endl;
		exit( 1 );
	    }
	    if ( !Conf::depots[tmp[0]].empty() &&
		 Conf::depots[tmp[0]] != tmp[1] ) {
		cout << "nodee: Ignoring depot "
		     << Conf::depots[tmp[0]]
		     << " for group "
		     << tmp[0]
		     << endl;
		dumpdepots = true;
	    }
	    Conf::depots[tmp[0]] = tmp[1];
	}
    }

    if ( dumpdepots ) {
	map<string,string>::iterator i = Conf::depots.begin();
	while ( i != Conf::depots.end() ) {
	    cout << "nodee: Group "
		 << i->first
		 << " uses depot "
		 << i->second
		 << endl;
	    ++i;
	}
    }

    if ( vm.count( "help" ) ) {
	cout << cli << endl;
	exit( 0 );
    }

    if ( geteuid() )
	cerr << "Nodee: Running as non-root. "
	        "All services will use the same UID as nodee."
	     << endl;

    Conf::scriptdir = "/usr/local/nodee/scripts";

    ZkClient zk( Conf::zk );

    Init i;

    HttpListener h6( HttpListener::V6, port, i );
    HttpListener h4( HttpListener::V4, port, i );

    if ( !h6.valid() && !h4.valid() ) {
	cerr << "nodee: Unable to listen to port "
	     << port
	     << " on either IPv4 or v6, exiting"
	     << endl;
	exit( 1 );
    }

    ChoreKeeper k( i );
    k.start();
}
