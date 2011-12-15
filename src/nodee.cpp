// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <stdio.h>
#include <stdlib.h>

#include "httplistener.h"
#include "chorekeeper.h"
#include "init.h"

#include <iostream>

#include <boost/program_options.hpp>


using namespace boost::program_options;


int main( int argc, char ** argv )
{
    int port;
    options_description desc( "Allowed options" );
    desc.add_options()
	( "help", "produce help message" )
	( "port,P", value<int>( &port )->default_value(40), 
	  "set nodee TCP port" )
	( "zookeeper", value<string>(), "input file" );

    variables_map vm;
    store( parse_command_line( argc, argv, desc ), vm );
    notify( vm ); 

    if ( vm.count( "help" ) ) {
	cout << desc << endl;
	exit( 0 );
    }

    HttpListener h6( HttpListener::V6, port );
    HttpListener h4( HttpListener::V4, port );

    if ( !h6.valid() && !h4.valid() ) {
	cerr << "nodee: Unable to listen to port " 
	     << port
	     << " on either IPv4 or v6, exiting"
	     << endl;
	exit( 1 );
    }

    Init i;

    ChoreKeeper k( i );
    k.start();
}
