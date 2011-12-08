// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <stdio.h>
#include <stdlib.h>

#include "httplistener.h"
#include "chorekeeper.h"
#include "init.h"

int main( int argc, char ** argv )
{
    if ( argc != 1 ) {
	fprintf( stderr, "nodee: No command-line arguments.\n" );
	exit( 1 );
    }

    HttpListener h6( HttpListener::V6, 4040 );
    HttpListener h4( HttpListener::V4, 4040 );

    if ( !h6.valid() && !h4.valid() ) {
	fprintf( stderr,
		 "nodee: "
		 "Unable to listen to port 40 on either IPv4 or v6, "
		 "exiting\n" );
	exit( 1 );
    }

    Init i;

    ChoreKeeper k;
    k.start();
}
