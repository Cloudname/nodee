// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "process.h"

#include <unistd.h>
#include <signal.h>

#include "uid.h"


/*! Constructs a Process to look out for some child process.

    The caller manually has to hand ownership over to Init.

    This class never kills or otherwise affects the child process, it
    merely records information about it, such as the
    expectedTypicalMemory() and value() of the process. Nodee uses
    this to adjust the kernel's OOM killer policies, so that if the
    host runs out of memory, the most likely process killed is a
    low-value server that occupied lots of RAM.

    If objects of this class were to own or otherwise act on the
    process they mimic, then Init couldn't copy and delete Process
    objects quite so freely. The ability to treat Process objects
    as values is central to Nodee's pointerlessness.
*/

Process::Process()
    : p( 0 ),
      faults( 0 ), prevFaults( 0 ),
      rss( 0 )
{
}


/*! Fork a child process, redirect its stdin/stdout/stderr
  appriopriately, and call start() in the child.
*/

void Process::fork()
{
    if ( p )
	return;

    int tmp = ::fork();
    if ( tmp < 0 ) {
	// an error. record the problem somehow, then just return.
	p = 0;
	return;
    } else if ( p == 0 ) {
	// we're in the child.
	if ( g )
	    setregid( g, g );
	if ( u )
	    setreuid( u, u );
	start();
    } else {
	// we're in the parent.
	p = tmp;
    }
}


/*! Notifies this object that it's process is gone, and how.

    The default implementation does nothing; a subclass may need to
    e.g. create a follow-on Process.

    Init will delete the Process after calling this.
*/

void Process::handleExit( int status, int signal )
{
    status = status;
    signal = signal;
}


/*! Called in the child process to start the child's work.

*/

void Process::start()
{

}


/*!

*/

void Process::launch( const ServerSpec & what )
{
    Process p;
}


/*! Constructs a copy of \a other. Deep copy, no sharing. */

Process::Process( const Process & other )
    : p( other.p ), s( other.s ),
      faults( other.faults ),
      prevFaults( other.prevFaults ),
      rss( other.rss )
{
}


void Process::operator=( const Process & other )
{
    p = other.p;
    s = other.s;
    faults = other.faults;
    prevFaults = other.prevFaults;
    rss = other.rss;
}


/*! Records \a r as the current RSS, in kbytes */

void Process::setCurrentRss( int r )
{
    rss = r;
}

/*! Returns the recorder RSS size, in kbytes */
int Process::currentRss() const
{
    return rss;
}


/*! Records that \a f faults have occured since time immemorial. */

void Process::setPageFaults( int x )
{
    prevFaults = faults;
    faults = x;
}


/*! Returns how many faults have occured between the last and second-to-last
    calls to setPageFaults().
*/

int Process::recentPageFaults() const
{
    return faults - prevFaults;
}


/*! Sets the object's state to look as though it has forked and the
    child's pid is \a fakepid. Used only for testing.
*/

void Process::fakefork( int fakepid )
{
    p = fakepid;
}


/*! Stops the process, either by calling the script specified in the
    ServerSpec or by killing it. If the latter, then the kill is
    rude. Anyone who wants a pleasant kill can supply a suitable
    script.
*/

void Process::stop()
{
    if ( !valid() )
	return;

    string script = s.shutdownScript();
    if ( script.empty() ) {
	::kill( 9, p );
    } else {
	// trouble here. need new functionality.  the uid used needs
	// to be visible to the c++, not assigned by sh at startup
	// time. foo.
    }
}


/*! Returns the UID used by this child, or 0 if the Process is not
    valid(). In theory, even valid() processes may run as root, but in
    practice that should not happen.
*/

int Process::uid() const
{
    return valid() ? u : 0;
}


/*! Returns the GID used by this child, or 0 if the Process is not
    valid(). In theory, even valid() processes may run as root, but in
    practice that should not happen.
*/

int Process::gid() const
{
    return valid() ? g : 0;
}


/*! Picks otherwise unused UID and GID for this process. */

void Process::assignUidGid()
{
    u = chooseFreeUid();
    g = chooseFreeGid();
}
