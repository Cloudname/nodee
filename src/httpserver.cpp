// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "httpserver.h"

#include <stdio.h>

#include <boost/lexical_cast.hpp>



/*! \class HttpServer httpserver.h
  
  The HttpServer class provdes a HTTP server for Nodee's RESTful
  API. It expects to run in a thread of its own; the start() function
  does all the work, then exits.
  
  I couldn't find embeddable HTTP server source I liked (technically
  plus BSD), so on the advice of James Antill, I applied some
  cut-and-paste. This server does not support the ASCII art from
  http://www.and.org/texts/server-http. It's even more restrictive than
  Apache, and MUCH more restrictive than RFC 2616.

  The main purpose (I hesitate to say benefit) of these restrictions
  is to avoid allocating memory. Memory never allocated is memory
  never leaked.

  The member functions may be sorted into three groups: start() is the
  do-it-all function (operator()() is a wrapper for start()),
  readRequest(), parseRequest(), readBody() and respond() contain the
  bulk of the code and are separated out for proper testing, and the
  four accessors operation(), path(), body() and and contentLength()
  exist for testing.
*/


/*! Constructs a new HttpServer for \a fd.
*/

HttpServer::HttpServer( int fd )
    : Server( fd )
{
    // nothing needed (yet?)
}


/*! Parses input, acts on it. Returns only in case of error.

    This function is not testable.
*/

void HttpServer::start()
{
    while ( true ) {
	readRequest();
	if ( valid() && contentLength )
	    readBody();

	if ( !valid() )
	    return;

	respond();
    }
}


/*! Reads and parses a single request; sets various member functions
 *  accordingly.

    The parser is quite amazingly strict when it does parse, but
    mostly it doesn't. The client can tell us what Content-Type it
    wants for the report about its RESTfulness, but we're don't
    atually care what it says, so we don't even parse its sayings. As
    I write these words, the only header field we really parse is
    Content-Length, which is necessary for POST.
*/

void HttpServer::readRequest()
{
    o = Invalid;
    cl = 0;
    p.erase();
    b.erase();
    h.erase();

    h.resize( 32769 );

    // first, we read the header, one byte at a time. this is
    // generally considered inefficient, but if we're going to spin up
    // a JVM as a result of this request, who cares about a few hundred
    // system calls more or less?
    bool done = false;
    int i = 0;
    while ( i < 32768 && !done ) {
	char x;
	int r = read( fd(), &x, 1 );
	if ( r < 0 ) {
	    // an error. we don't care.
	    close();
	    return;
	}

	if ( r > 0 && x == 0 ) {
	    // some fun-loving client sent us a null byte. we have no
	    // patience with such games.
	    close();
	    return;
	}

	h.append( x );
	i++;

	// there are two ways to end a header: LFLF and CRLFCRLF
	if ( i >= 2 && h[i-1] == 10 && h[i-2] == 10 )
	    done = true;
	if ( i >= 3 && h[i-1] == 10 && h[i-2] == 13 && h[i-3] == 10 )
	    done = true; // LFCRLF. arguably even that's allowed.

	if ( i >= 32768 && !done ) {
	    // the sender sent 32k and didn't actually send a valid header.
	    // is the client buggy, blackhat or just criminally talkative?
	    close();
	    return;
	}
    }
}



/*! Parses the request supplied by readRequest(). May set operation()
    to Invalid, but does nothing else to signal errors.
*/

void HttpServer::parseRequest()
{
    if ( !h.compare( 0, 4, "GET " ) ) {
	operation = Get;
    } else if ( !h.compare( 0, 5, "POST " ) ) {
	operation = Post;
    } else {
	operation = Invalid;
    }

    int p = 0;
    int l = h.size();
    while( p < l && h[p] != ' ' )
	p++;
    while( p < l && h[p] == ' ' )
	p++;
    int s = p;
    while( p < l && h[p] != ' ' && h[p] != 10 )
	p++;
    
    path = h.substr( s, p-s );

    if ( operation != Post )
	return;

    // we need content-length. it's entirely case-insensitive, so we
    // can smash case.
    std::transform( h.begin(), h.end(), h.begin(), ::tolower() );
    size_type pos = h.find( "\ncontent-length:" );
    if ( pos == string::npos )
	return;
    p = pos + 16;
    s = p;
    while( p < l && h[p] == ' ' )
	p++;
    while ( headers[p] >= '0' && headers[p] <= '9' )
	p++;
    cl = boost::lexical_cast<int>( h.substr( s, p-s ) );
}


/*! Reads a body, for POST.
  
    On return, either the object will be valid() and the body() set,
    or the object will not be valid().
*/

void HttpServer::readBody()
{
    if ( !contentLength )
	return;

    body = new char[contentLength];
    int l = 0;
    while ( l < contentLength ) {
	int r = ::read( fd(), l+*a, contentLength-l );
	if ( r < 0 ) {
	    close();
	    delete body;
	    body = 0;
	    return;
	}
	l += r;
    }
}


/*! Responds to the request, such as it is.
  
    Effectively untestable. Could be separated out into smaller
    chunks, but I don't care right now.
*/

void HttpServer::respond()
{
    if ( o == Invalid ) {
	send( httpResponse( 400, "text/plain", "Utterly total parse error" ) );
	return;
    }

    if ( o == Post && path == "/process/launch" ) {
	ServerSpec s( body );
	if ( !s.valid() ) {
	    send( httpResponse( 400, "text/plain",
				"Parse error for the JSON body" ) );
	} else {
	    Launcher l( s );
	    boost::thread( l );
	    send( httpResponse( 200, "text/plain",
				"Will launch, or try to" ) );
	}
	return;
    }
    
    if ( o == Post ) {
	send( httpResponse( 404, "text/plain",
			    "No such response" ) );
	return;
    }
    
    // it's Get
    
}
