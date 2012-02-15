// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "httplistener.h"

#include "httpserver.h"

#include <boost/thread.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>



/*! \class HttpListener httplistener.h

  The HttpListener class listens for connections from clients on a
  single socket, and when one arrives, it creates a suitable
  HttpServer in a thread of its own.

  HttpListener has one particular weakness: Since it listens on one
  socket, if nodee is run in a mixed ipv4/6 environment, nodee has to
  create two HttpListener objects. Creating two may however fail,
  leaving nodee with the unenviable task of finding out whether the
  failure is innocent or serious. I didn't like putting ::exit() calls
  in HttpListener, so I put all that logic in main.
*/


/*! Constructs an HTTP listener for \a port using \a family (V4 or V6),
    creating HttpServers connected to \a i when clients connect.
*/

HttpListener::HttpListener( HttpListener::Family family, int port, Init & i )
    : init( i )
{
    int retcode;

    if ( family == V4 ) {
        f = ::socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	int i = 1;
	::setsockopt( f, SOL_SOCKET, SO_REUSEADDR, &i, sizeof (int) );

	struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons( port );
        addr.sin_addr.s_addr = htonl( INADDR_ANY );

	retcode = ::bind( f, (struct sockaddr *)&addr, sizeof( addr ) );
    }
    else {
        f = ::socket( AF_INET6, SOCK_STREAM, IPPROTO_TCP );

	int i = 1;
	::setsockopt( f, SOL_SOCKET, SO_REUSEADDR, &i, sizeof (int) );

	struct sockaddr_in6 addr;
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons( port );
	i = 0;
	while ( i < 8 ) {
	    addr.sin6_addr.s6_addr16[i] = 0;
	    i++;
	}

	retcode = ::bind( f, (struct sockaddr *)&addr, sizeof( addr ) );
    }

    if ( retcode < 0 ) {
	::close( f );
	f = -1;
	return;
    }

    // we try to make the queue longer so that we can cope well with
    // connection spikes, but if that fails we don't even sulk, we
    // just go on.
    (void)::listen( f, 64 );

    boost::thread( *this );
    // this lets the thread run unmanaged; when run() exits the object
    // will be deallocated.
}


/*! Called to start the thread. Does all that the listener needs, which
    doesn't involve very many lines of code.

    Never exits, which seems like a modest problem. I don't see any
    reason why the

*/

void HttpListener::start()
{
    while( f >= 0 ) {
	int i = ::accept( f, 0, 0 );
	if ( i >= 0 )
	    boost::thread( HttpServer( i, init ) );
	else if ( errno != EAGAIN )
	    f = -1;
    }
}



/*! Returns true if the listener actually is listening to something,
    and false if some problem prevents it from achieving anything.
*/

bool HttpListener::valid() const
{
    return f > 1;
}


/*! boost::thread wants to call start() by this name, so here's a
    wrapper around start().
*/

void HttpListener::operator()()
{
    start();
}
