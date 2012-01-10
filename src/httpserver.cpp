// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "httpserver.h"

#include "serverspec.h"
#include "service.h"
#include "process.h"

#include <stdio.h>

#include <algorithm>

#include <boost/lexical_cast.hpp>


/*! \class HttpServer httpserver.h

  The HttpServer class provdes a HTTP server for Nodee's API. It
  expects to run in a thread of its own; the start() function does all
  the work, then exits.

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


/*! Constructs a new HttpServer for \a fd, working on \a i.
*/

HttpServer::HttpServer( int fd, Init & i )
    : init( i ), o( Invalid ), cl( 0 ), f ( fd )
{
    // nothing needed (yet?)
}


/*! Parses input, acts on it. Returns only in case of error.

    This function is not testable.
*/

void HttpServer::start()
{
    while ( true ) {
	parseRequest( readRequest() );
	if ( f >= 0 && cl > 0 )
	    readBody();

	if ( f < 0 )
	    return;

	respond();
    }
}


/*! Reads and returns a single request. Throws nothing.

    Aborts after 32k; the common requests will be <500 bytes and
    practically all <2k, so 32k is a good sanity limit.


*/

string HttpServer::readRequest()
{
    // first, we read the header, one byte at a time. this is
    // generally considered inefficient, but if we're going to spin up
    // a JVM as a result of this request, who cares about a few hundred
    // system calls more or less?
    string s;

    bool done = false;
    int i = 0;
    while ( i < 32768 && !done ) {
	char x[2];
	int r = ::read( f, &x, 1 );
	if ( r < 0 ) {
	    // an error. we don't care.
	    close();
	    return string();
	}

	if ( r > 0 && x == 0 ) {
	    // some fun-loving client sent us a null byte. we have no
	    // patience with such games.
	    close();
	    return string();
	}

	x[1] = 0;
	s += x;
	i++;

	// there are two ways to end a header: LFLF and CRLFCRLF
	if ( i >= 2 && s[i-1] == 10 && s[i-2] == 10 )
	    done = true;
	if ( i >= 3 && s[i-1] == 10 && s[i-2] == 13 && s[i-3] == 10 )
	    done = true; // LFCRLF. arguably even that's allowed.

	if ( i >= 32768 && !done ) {
	    // the sender sent 32k and didn't actually send a valid header.
	    // is the client buggy, blackhat or just criminally talkative?
	    close();
	    return string();
	}
    }

    return s;
}



/*! Parses \a h as a HTTP request. May set operation() to Invalid, but
    does nothing else to signal errors.

    The parser is quite amazingly strict when it does parse, but
    mostly it doesn't. The client can tell us what Content-Type it
    wants for the report about its RESTfulness, but we're don't
    atually care what it says, so we don't even parse its sayings. As
    I write these words, the only header field we really parse is
    Content-Length, which is necessary for POST.
*/

void HttpServer::parseRequest( string h )
{
    o = Invalid;
    cl = 0;
    p.erase();
    b.erase();

    if ( !h.compare( 0, 4, "GET " ) ) {
	o = Get;
    } else if ( !h.compare( 0, 5, "POST " ) ) {
	o = Post;
    } else {
	o = Invalid;
    }

    int n = 0;
    int l = h.size();
    while( n < l && h[n] != ' ' )
	n++;
    while( n < l && h[n] == ' ' )
	n++;
    int s = n;
    while( n < l && h[n] != ' ' && h[n] != 10 )
	n++;

    p = h.substr( s, n-s );

    if ( o != Post )
	return;

    // we need content-length. it's entirely case-insensitive, so we
    // can smash case and ignore non-ascii.
    std::transform( h.begin(), h.end(), h.begin(), ::tolower );
    size_t pos = h.find( "\ncontent-length:" );
    if ( pos == string::npos )
	return;
    n = pos + 16;
    while( n < l && h[n] == ' ' )
	n++;
    s = n;
    while ( n < l && h[n] >= '0' && h[n] <= '9' )
	n++;
    cl = boost::lexical_cast<int>( h.substr( s, n-s ) );
}


/*! Reads a body, for POST.

    On return, either the object will be valid() and the body() set,
    or the object will not be valid().
*/

void HttpServer::readBody()
{
    if ( !cl )
	return;

    char * tmp = new char[cl+1];
    int l = 0;
    while ( l < cl ) {
	int r = ::read( f, l+tmp, cl-l );
	if ( r < 0 ) {
	    close();
	    delete[] tmp;
	    return;
	}
	l += r;
    }
    tmp[cl] = '\0';
    b = tmp;
    delete[] tmp;
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
    
    // start, stop, list services
    // install, uninstall, list artifacts

    if ( o == Post && p == "/service/start" ) {
	ServerSpec s = ServerSpec::parseJson( b );
	if ( !s.valid() ) {
	    send( httpResponse( 400, "text/plain",
				"Parse error for the JSON body" ) );
	} else {
	    Process::launch( s );
	    send( httpResponse( 200, "text/plain",
				"Will launch, or try to" ) );
	}
	return;
    }

    if ( o == Post && p.substr( 0, 14 ) == "/service/stop/" ) {
	Process s;
	try {
	    s = init.find( boost::lexical_cast<int>( p.substr( 14 ) ) );
	} catch ( boost::bad_lexical_cast ) {
	}
	if ( !s.valid() ) {
	    send( httpResponse( 400, "text/plain",
				"No such service" ) );
	} else {
	    // hmm?
	    close();
	}
	return;
    }

    if ( o == Post && p.substr( 18 ) == "/artifact/install/" ) {
	ServerSpec s = ServerSpec::parseJson( b );
	if ( !s.valid() ) {
	    send( httpResponse( 400, "text/plain",
				"Parse error for the JSON body" ) );
	} else {
	    Process::launch( s );
	    send( httpResponse( 200, "text/plain",
				"Will launch, or try to" ) );
	}
	return;
    }

    if ( o == Post && p.substr( 0, 20 ) == "/artifact/uninstall/" ) {
	string artifact = p.substr( 21 );
	// ARNT
	send( httpResponse( 200, "text/plain",
				"Will uninstall, or try to" ) );
	return;
    }

    if ( o == Post ) {
	send( httpResponse( 404, "text/plain",
			    "No such response" ) );
	return;
    }

    // it's Get

    if ( p == "/service/list" ) {
	string o( httpResponse( 200, "application/json",
				"Service list follows" ) );
	o.append( Service::list( init ) );
	send( o );
    }

    if ( p == "/artifact/list" ) {
	string o( httpResponse( 200, "application/json",
				"Artifact list follows" ) );
	o.append( Service::list( init ) );
	send( o );
    }

    if ( p == "/" ) {
	string r = httpResponse( 200, "text/html",
				 "This is not a web site" );
	r += "<html>"
	     "<head><title>Nodee</title><head>"
	     "<body style='text-align: center;'>"
	     "<h1>Nodee</h1>"
	     "<p>This is the home page of a nodee server. "
	     "There are no web pages to see here, only a few JSON "
	     "API things, and those aren't really something you'll "
	     "want to look at, if you understand."
	     "<p>Have a look at the "
	     "<a href=\"http://cloudname.org\">Cloudname</a> "
	     "home page or perhaps the "
	     "<a href=\"https://github.com/Cloudname/nodee\">Nodee source</a> "
	     "instead, that'll be much more fun."
	     "<p><img src=\"http://rant.gulbrandsen.priv.no/images/under-construction.gif\">"
	     "</body>"
	     "</html>\n";
	send( r );
    }
    
    if ( p == "/robots.txt" ) {
	string r = httpResponse( 200, "text/plain",
				 "This is not a web site" );
	r += "User-Agent: *\r\nDisallow: /\r\n";
	send( r );
    }
    
    if ( p == "/sitemap.xml" ) {
	string r = httpResponse( 200, "application/xml",
				 "This is not a web site" );
	r += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
	     "<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\"\n"
	     "</urlset nicetry=true>\n";
	send( r );
    }
    
    send( httpResponse( 404, "text/plain", "No such page" ) );
}


/*! Closes the socket and updates the state machine as needed. */

void HttpServer::close()
{
    ::close( f );
    f = -1;
}


/*! Returns a HTTP response string with \a numeric status, \a textual
    explanation and \a contentType.

    This function does most of what send() ought to do, but this is
    easily testable and the same logic isend() would not be.
*/

string HttpServer::httpResponse( int numeric, const string & contentType,
				 const string & textual )
{
    string r;
    // we blithely assume that 100<=numeric<=999
    r += boost::lexical_cast<string>( numeric );
    r += " ";
    r += textual;
    r += "\r\n"
	 "Connection: close\r\n"
	 "Server: nodee\r\n"
	 "Content-Type: ";
    r += contentType;
    r += "\r\n\r\n";
    return r;
}


/*! Sends \a response. This function is untested, borderline
    untestable, which is why it's simple.
*/

void HttpServer::send( string response )
{
    int o = 0;
    int l = response.length();
    while ( o < l ) {
	int r = ::write( f, o + response.data(), l - o );
	if ( r <= 0 ) {
	    close();
	    return;
	}
	o += r;
    }
    close();
}
