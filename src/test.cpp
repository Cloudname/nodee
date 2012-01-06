// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#define BOOST_TEST_MODULE HttpServer test

#include <boost/test/included/unit_test.hpp>

#include "httpserver.h"

BOOST_AUTO_TEST_CASE( foo )
{
    HttpServer x( 0 );

    x.parseRequest( "GET / HTTP/1.42\n\n" );
    BOOST_CHECK_EQUAL( x.operation(), HttpServer::Get );
    BOOST_CHECK_EQUAL( x.path(), "/" );

    x.parseRequest( "GET /x HTTP/1.42\n\n" );
    BOOST_CHECK_EQUAL( x.operation(), HttpServer::Get );
    BOOST_CHECK_EQUAL( x.path(), "/x" );

    x.parseRequest( "POST /asdf/ HTTP/1.42\nContent-Length: 1000000\n\n" );
    BOOST_CHECK_EQUAL( x.operation(), HttpServer::Post );
    BOOST_CHECK_EQUAL( x.path(), "/asdf/" );
    BOOST_CHECK_EQUAL( x.contentLength(), 1000000 );
}

// EOF
