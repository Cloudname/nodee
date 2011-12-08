// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "chorekeeper.h"

#include <unistd.h>
#include <iostream>
#include <fstream>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>


using namespace std;


/*! \class ChoreKeeper chorekeeper.h

    The ChoreKeeper class regularly performs various chores, such as
    checking for RAM/CPU overload.

    There is no configuration; the class just does the right thing.
*/



/*! Constructs a ChoreKeeper. The caller has to call start() to
    perform chores.
*/

ChoreKeeper::ChoreKeeper()
{
}


/*! The guts of the class. */

void ChoreKeeper::start()
{
    while( true ) {
	sleep( 1 );
	detectThrashing();
	if ( isThrashing() ) {
	    
	}
    }
}


/*! Reads the memory status using /proc */

void ChoreKeeper::readMemoryStatus()
{

}


/*!

*/

void ChoreKeeper::readCpuStatus()
{
    
}


/*!

*/

void ChoreKeeper::adjustProcesses()
{
    
}


/*! Just a dummy to please the linker. Yes, really. */

ChoreKeeper::~ChoreKeeper()
{
}


/*! This function looks at whether the host appears to be thrashing,
    and stores the result in instance variables.
 
    It's a little difficult to define thrashing. I tried to
    experiment, but gathering data during thrashing is such a pain.

    Generally two of the following properties were true for many
    seconds: The machine was reading in pages (major page faults), the
    machine was writing to swap, the load was far higher than the
    number of cores. But any one of these can also be true briefly at
    times when my human judgment is that the machine isn't thrashing.

    This function does a heuristic momentary test. If eight
    consecutive tests indicate thrashing, isThrashing() returns true.
*/

void ChoreKeeper::detectThrashing()
{
    int nr_free_pages = 0; // pages currently unused
    int pgmajfault = 0; // times a process has had to wait for a page from disk
    int pgpgout = 0; // times something has been written to disk

    readProcFile( "/proc/vmstat", nr_free_pages, pgmajfault, pgpgout );

    int n = 7;
    while ( n > 0 ) {
	thrashing[n] = thrashing[n-1];
	n--;
    }
    
    // heuristic hell here

    // rule 1. we're not threashing
    thrashing[0] = false;

    // rule 2. if we have megabytes of unused RAM, we can't be
    // thrashing.
    if ( nr_free_pages > 5000 )
	return;

    // rule 3. if we're paging in anything, we are thrashing.
    if ( pgmajfault > 3 ) {
	// 3 is very low, but it only applies when we're out of RAM,
	// and isThrashing() will ensure that we have to be paging in
	// in eight consecutive seconds, so I think a low threshold is
	// good.
	thrashing[0] = true;
	return;
    }

    // rule 4. if we aren't writing, we aren't thrashing.
    if ( pgpgout < 3 ) {
	// this is tricky, and perhaps not good. if we're out of RAM
	// (see rule 2) but aren't paging in anything (see rule 3)
	// then being out of RAM can't be a real problem. right?
	return;
    }

    thrashing[0] = true;
}


/*! Returns true if the machine appears to thrash, and has been for a
    few seconds. Returns false in all other cases (including in the
    first few seconds after start).
*/

bool ChoreKeeper::isThrashing() const
{
    int n = 8;
    while ( n > 0 )
	if ( !thrashing[--n] )
	    return false;
    return true;
}


/*! Opens and reads \a fileName, storing the eponymous variables in \a
    nr_free_pages, \a pgmajfault and \a pgpgout.
*/

void ChoreKeeper::readProcFile( const char * fileName,
				int & nr_free_pages,
				int & pgmajfault,
				int & pgpgout )
{
    ifstream vmstat( fileName );
    nr_free_pages = 0; // pages currently unused
    pgmajfault = 0; // times a process has had to wait for a page from disk
    pgpgout = 0; // times something has been written to disk
    while ( vmstat ) {
	string line;
	getline( vmstat, line );
	boost::tokenizer<> t( line );
	boost::tokenizer<>::iterator i = t.begin();
	string n = *i;
	++i;
	int v = boost::lexical_cast<int>( *i );

	// nr_free_pages is the number of RAM pages that are
	// completely unused.
	if ( n == "nr_free_pages" )
	    nr_free_pages = v;
	// pgmajfault is the number of times a process has had to wait
	// for a page to be read from either swap or the executable
	if ( n == "pgmajfault" )
	    pgmajfault = v;
	// pgpgout is the number of things that have been written to
	// disk, including swap but also including everything else
	if ( n == "pgpgout" )
	    pgpgout = v;

	// I use pgmajfault for input since that's about waiting, and
	// waiting is the most important effect 
    }
}
