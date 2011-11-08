// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "init.h"


/*! \class Init init.h
  
  The Init class manages all the subprocesses. Nothing to it, really.
*/



/*! Constructs an empty Init. */

Init::Init()
{
    boost::thread( *this );
}


/*! Destroys the object. Does not kill any subprocesses. */

Init::~Init()
{
    // nothing needed
}


/*! Does all there is to do. */

void Init::start()
{
    while ( true )
	check();
}


/*! Waits for and processes a single child event. */

void Init::check()
{
    int status;
    int pid = ::wait( &status );

    if ( pid < 0 ) {
	// an error. what to do. exit nodee?
    }

    if ( pid <= 0 )
	return;

    // we now have a pid. find out what happened to it.
    int exitStatus = -1;
    if ( WIFEXITED( status ) )
	exitStatus = WEXITSTATUS( status );
    bool signalled = WIFSIGNALED( status );
    int signal = 0;
    if ( signalled )
	signal = WTERMSIG( status );

    // find the relevant Process object, ping it and forget about it.
    std::list<Process>::iterator i = l.begin();
    while ( i != l.end() && i->pid() != pid )
	++i;
    if ( i != l.end() && i->pid() == pid ) {
	i->handleExit( exitStatus, signal );
	l.remove( i );
    }
}
