// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "chorekeeper.h"

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sysexits.h>

#include <iostream>
#include <fstream>

#include <map>
#include <list>

#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
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

ChoreKeeper::ChoreKeeper( Init & i )
    : init( i )
{
    int n = 7;
    while ( n > 0 ) {
	thrashing[n] = false;
	n--;
    }
}


/*! The guts of the class. */

void ChoreKeeper::start()
{
    while( true ) {
	::sleep( 1 );
	scanProcesses( "/proc", getpid() );
	detectThrashing();
	if ( isThrashing() ) {
	    Process jesus;
	    jesus = furthestOverPeak();
	    if ( !jesus.valid() )
		jesus = furthestOverExpected();
	    if ( !jesus.valid() )
		jesus = leastValuable();
	    if ( !jesus.valid() )
		jesus = biggest();
	    if ( jesus.valid() ) {
		// we kill with signal 9, since we're already in a bad  state.
		::kill( jesus.pid(), 9 );
		// come to think of it, should we use Process::stop()?
		
		// but once that's done, we record that we're NOT
		// thrashing, since it's quite likely that even after
		// we've killed a process, others will need to page in
		// their data, and we don't want to react to that
		// activity by killing more processes.
		thrashing[0] = false;
	    }
	}
    }
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

    readProcVmstat( "/proc/vmstat", nr_free_pages, pgmajfault, pgpgout );

    int n = 7;
    while ( n > 0 ) {
	thrashing[n] = thrashing[n-1];
	n--;
    }

    thrashing[0] = oneBitOfThrashing( nr_free_pages, pgmajfault, pgpgout );
}


bool ChoreKeeper::oneBitOfThrashing( int nr_free_pages, int pgmajfault, int pgpgout )
{
    // heuristic hell here

    // rule 1. if we have megabytes of unused RAM, we can't be
    // thrashing.
    if ( nr_free_pages > 5000 )
	return false;

    // rule 2. if we're paging in anything, we are thrashing.
    if ( pgmajfault > 3 ) {
	// 3 is very low, but it only applies when we're out of RAM,
	// and isThrashing() will ensure that we have to be paging in
	// in eight consecutive seconds, so I think a low threshold is
	// good.
	return true;
    }

    // rule 3. if we aren't writing, we aren't thrashing.
    if ( pgpgout < 3 ) {
	// this is tricky, and perhaps not good. if we're out of RAM
	// (see rule 2) but aren't paging in anything (see rule 3)
	// then being out of RAM can't be a real problem. right?
	return false;
    }

    return true;
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

void ChoreKeeper::readProcVmstat( const char * fileName,
				  int & nr_free_pages,
				  int & pgmajfault,
				  int & pgpgout )
{
    ifstream vmstat( fileName );
    nr_free_pages = 0; // pages currently unused
    pgmajfault = 0; // times a process has had to wait for a page from disk
    pgpgout = 0; // times something has been written to disk
    boost::char_separator<char> space( " " );
    while ( vmstat ) {
	string line;
	getline( vmstat, line );
	if ( !line.empty() ) {
	boost::tokenizer<boost::char_separator<char> > t( line, space );
	boost::tokenizer<boost::char_separator<char> >::iterator i = t.begin();
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
	// waiting is the most important effect of thrashing
        }
    }
}

RunningProcess ChoreKeeper::parseProcStat( string line )
    throw ( boost::bad_lexical_cast )
{
    // the first four fields are pid, filename in parens,
    // state and ppid. we have to get rid of the filename
    // so it won't confuse the tokenizer (which wants
    // space-separated thingies).

    int i = 0;
    while ( i < line.length() && line[i] != '(' )
        i++;
    while ( i < line.length() && line[i] != ')' ) {
        if ( line[i] == '\\' ) // the kernel escapes rightparens
	    line[i++] = '0';
        line[i++] = '0';
    }
    if ( line[i] == ')' )
        line[i] = '0';

    boost::tokenizer<> tokens( line );
    boost::tokenizer<>::iterator t = tokens.begin();
    boost::tokenizer<>::iterator end = tokens.end();


    RunningProcess r;
    if ( t == end )
	return RunningProcess();
    r.pid = boost::lexical_cast<int>( *t );
    ++t; // points to the nulls from above
    ++t; // points to the state ('D', 'R' or whatever)
    ++t; // points to the ppid
    if ( t == end )
	return RunningProcess();
    r.ppid = boost::lexical_cast<int>( *t );
    ++t; // points to the process group
    ++t; // points to the session id
    ++t; // points to the tty number
    ++t; // points to the process group controller
    ++t; // points to the kernel flags
    ++t; // points to minflt
    ++t; // points to cminflt
    ++t; // points to majflt
    if ( t == end )
	return RunningProcess();
    r.majflt = boost::lexical_cast<int>( *t );
    ++t; // points to cmajflt
    if ( t == end )
	return RunningProcess();
    r.majflt += boost::lexical_cast<int>( *t );
    ++t; // points to user time ticks
    ++t; // points to kernel time ticks
    ++t; // points to waited-for child user time
    ++t; // points to waited-for child kernel time ticks
    ++t; // points to kernel real-time priority
    ++t; // points to niceness
    ++t; // points to numthreads
    ++t; // points to null
    ++t; // points to the process' start time
    ++t; // points to vsize
    ++t; // points to rss in pages
    if ( t == end )
	return RunningProcess();
    r.rss = boost::lexical_cast<int>( *t );
    return r;
}


/* ubuntu 10.04 includes boost 1.40, which has boost::filesystem API
   2. 11.10 includes API 3. rather than demand and upgraded or
   downgraded boost, I tried to write code that works with both. this
   wrapper is needed to get a filename from an iterator.
*/

static string filename( const boost::filesystem::directory_iterator & i )
{
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 3
    return i->path().native();
#else
    return p.filename();
#endif
}

/*! Scans the Process table and the /proc/<pid>/stat files and finds out
    how much memory each of our processes is using (including all children)
    and how badly it is suffering from thrashing.

    \a proc is /proc (or another value for testing) and \a me is
    nodee's pid (or another value for testing). I dislike this,
    can't tell why.
*/

void ChoreKeeper::scanProcesses( const char * proc, int me )
{
    using namespace boost::filesystem;

    path p ( proc );
    map<int,RunningProcess> observed;
    try {

	directory_iterator i = directory_iterator( p );
	while ( i != directory_iterator() ) {
	    if ( *filename( i ).rbegin() <= '9' && is_directory( p ) ) {
		string x = filename( i );
		x.append( "/stat" );
		ifstream stat( x.data() );
		string line;
		getline( stat, line );
		try {
		    RunningProcess r( parseProcStat( line ) );
		    observed[r.pid] = r;
		} catch (  boost::bad_lexical_cast ) {
		    // if parseProcStat throws, then we just don't
		    // manage that process
		}
	    }
	    ++i;
	}
    } catch (const filesystem_error& ex) {
	// kill all processes or just fail?
	::exit( EX_SOFTWARE );
    }

    map<int,RunningProcess>::iterator i = observed.begin();
    while ( i != observed.end() ) {
	pid_t mother = i->second.pid;
	while ( mother &&
	        observed[mother].ppid &&
	        observed[mother].ppid != me )
	    mother = observed[mother].ppid;
	if ( observed[mother].pid != i->second.pid ) {
	    observed[mother].rss += i->second.rss;
	    observed[mother].majflt += i->second.majflt;
	}
	++i;
    }

    list<Process> & pl = init.processes();
    list<Process>::iterator m( pl.begin() );
    while ( m != pl.end() ) {
	m->setCurrentRss( observed[m->pid()].rss );
	m->setPageFaults( observed[m->pid()].majflt );
	++m;
    }
}


/*! Scans the Process table and finds the process whose memory usage
    is furthest above its stated peak. Returns an invalid Process if
    none are above their peak.
*/

Process ChoreKeeper::furthestOverPeak() const
{
    Process p;
    list<Process> & pl = init.processes();
    list<Process>::iterator m( pl.begin() );
    while ( m != pl.end() ) {
	int over = m->currentRss() - m->expectedPeakMemory();
	if ( over > 0 &&
	     ( !p.valid() ||
	       over > p.currentRss() - p.expectedPeakMemory() ) )
	    p = *m;
	++m;
    }

    return p;
}

/*! Scans the Process table and finds the process whose memory usage
    is furthest above it stated typical memory usage. Returns an invalid
    Process if none are above their expected typical size.
*/

Process ChoreKeeper::furthestOverExpected() const
{
    Process p;
    list<Process> & pl = init.processes();
    list<Process>::iterator m( pl.begin() );
    while ( m != pl.end() ) {
	int over = m->currentRss() - m->expectedTypicalMemory();
	if ( over > 0 &&
	     ( !p.valid() ||
	       over > p.currentRss() - p.expectedTypicalMemory() ) )
	    p = *m;
	++m;
    }

    return p;
}


/*! Scans the Process table and finds the least important
    process. Returns an invalid Process if none are less important
    than the most important Process.
*/

Process ChoreKeeper::leastValuable() const
{
    Process max;
    Process min;
    list<Process> & pl = init.processes();
    list<Process>::iterator m( pl.begin() );
    while ( m != pl.end() ) {
	if ( !max.valid() || max.value() < m->value() )
	    max = *m;
	if ( !min.valid() || min.value() > m->value() )
	    min = *m;
	++m;
    }
    if ( min.valid() && min.value() < max.value() )
	return min;
    return Process();
}


/*! Scans the Process table and finds the Process that's most negatively
    affected by thrashing. Returns an invalid Process if none are noticeably
    worse affected than the others.
*/

Process ChoreKeeper::thrashingMost() const
{
    Process p;
    list<Process> & pl = init.processes();
    list<Process>::iterator m( pl.begin() );
    while ( m != pl.end() ) {
	if ( m->recentPageFaults() > p.recentPageFaults() )
	    p = *m;
	++m;
    }

    return p;
}


/*! Scans the Process table and finds the process whose memory usage
    is biggest. Returns an invalid Process only if no Processes are
    being managed by Nodee.
*/

Process ChoreKeeper::biggest() const
{
    Process p;
    list<Process> & pl = init.processes();
    list<Process>::iterator m( pl.begin() );
    while ( m != pl.end() ) {
	if ( !p.valid() || m->currentRss() > p.currentRss() )
	    p = *m;
	++m;
    }

    return p;
}
