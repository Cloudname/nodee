// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "httplistener.h"

#include <boost/thread.hpp>

#include <sys/types.h>
#include <sys/socket.h>



/*! \class HttpListener httplistener.h

  The HttpListener class listens for connections from clients, and
  when one arrives, it creates a suitable HttpServer in a thread of
  its own.
*/


/*! Constructs an HTTP listener for \a port using \a family (V4 or V6).

  After construction, the fd() is ready to watch.
*/

HttpListener::HttpListener( Family family, int port )
{
    int retcode;

    if ( family == V4 ) {
        setFd( ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP ) );

	int i = 1;
	::setsockopt( f, SOL_SOCKET, SO_REUSEADDR, &i, sizeof (int) );

	struct sockaddr_in addr;
        addr.in.sin_family = AF_INET;
        addr.in.sin_port = htons( port );
        addr.in.sin_addr.s_addr = htonl( d->ip4a );

	retcode = ::bind( fd(), (struct sockaddr *)&addr, sizeof( addr ) );
    }
    else {
        setFd( ::socket( AF_INET6, SOCK_STREAM, IPPROTO_TCP ) );

	int i = 1;
	::setsockopt( f, SOL_SOCKET, SO_REUSEADDR, &i, sizeof (int) );

	struct sockaddr_in6 addr;
	addr.in6.sin6_family = AF_INET6;
	addr.in6.sin6_port = htons( port );
	int i = 0;
	while ( i < 8 ) {
	    addr.in6.sin6_addr.s6_addr16[i] = 0;
	    i++;
	}

	retcode = ::bind( fd(), (struct sockaddr *)&addr, sizeof( addr ) );
    }

    if ( retcode < 0 ) {
        if ( errno == EADDRINUSE ) {
	    fprintf( stderr,
		     "Cannot listen to port %d "
		     "because another process is occupying it", port );
	    return;
        }
    }

    if ( ::listen( d->fd, 64 ) < 0 ) {
	log( "listen( " + fn( d->fd ) + ", 64 ) for port " + fn( port() ) +
	     " ) returned errno " + fn( errno ), Log::Debug );
	return;
    }

    boost::thread( *this );
    // this lets the thread run unmanaged; when run() exits the object
    // will be deallocated and 
}


/*! Called to start the thread. Does all that the listener needs, and
    finally cleans up and exits. */

void HttpListener::run()
{
    while( valid() ) {
	int i = ::accept( fd(), 0, 0 );
	if ( i < 0 ) {
	    fprintf( "foo\n", errno );
	    return;
	}
	boost::thread( HttpServer( i ) );
    }
}
 
