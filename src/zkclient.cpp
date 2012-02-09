// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "zkclient.h"

#include "hoststatus.h"

#include <sysexits.h>

#include <string>

#include <unistd.h> // gethostname()

using namespace std;

#include <boost/thread.hpp>

static bool initialised;
static boost::mutex zkmutex; // we don't need it, but we must fill in forms
static boost::condition_variable zkwaiter; // what we do need


static void watcher( zhandle_t * zzh,
		     int type, int state,
		     const char *, void* )
{
    if ( type != ZOO_SESSION_EVENT )
	return;

    boost::lock_guard<boost::mutex> lock( zkmutex );
    if ( state == ZOO_CONNECTED_STATE ||
	 state == ZOO_AUTH_FAILED_STATE ||
	 state == ZOO_EXPIRED_SESSION_STATE )
	::initialised = true;
    ::zkwaiter.notify_one();
}




/*!  Constructs an empty

*/

ZkClient::ZkClient( const std::string & server )
    : zh( 0 )
{
    int r;

    // for the moment, _DEBUG. I think this ought probably to be set
    // based on our cli arguments.
    ::zoo_set_debug_level( ZOO_LOG_LEVEL_DEBUG );

    // I dare not investigate why this is not the default. I am so
    // afraid that the result might ruin my entire day.
    ::zoo_deterministic_conn_order( 1 );

    // huh. I wonder.
    zh = ::zookeeper_init( server.c_str(), // the server we want
			   watcher, // a function we have to pass, oh no
			   30000, // receive timeout, unit undocumented
			   0, // our previous client id, 0 means none
			   0, // context object, e.g. this
			   0 ); // reserved for future expansion, huh, huh

    // next, wait for initialisation to be complete (or fail).
    boost::unique_lock<boost::mutex> lock( zkmutex );
    while ( !::initialised )
	zkwaiter.wait( lock );
    
    // zookeeper wants to tell us things. we don't want to listen.
    // still, we need a sacrificial buffer.
    char tmp[1025];

    // create the ephemeral node's parent just in case noone has done
    // it yet. we resolutely ignore all errors while we create this
    // node. if the node already exists, we're happy. if we can't
    // connect, the next zoo_create() will also fail and we'll handle
    // the problem then, etc.
    path = "/nodee";
    (void)zoo_create( zh,
		    path.c_str(),
		    "", -1,
		    &ZOO_OPEN_ACL_UNSAFE,
		    0,
		    tmp, 1024 );

    // now create the ephemeral node for this nodee
    path += "/";
    ::gethostname( tmp, 1024 );
    tmp[1024] = 0;
    path += tmp; // by this time path is right for start() to use
    string status = HostStatus();
    r = zoo_create( zh,
		    path.c_str(),
		    status.c_str(), status.length(),
		    &ZOO_OPEN_ACL_UNSAFE,
		    ZOO_EPHEMERAL,
		    tmp, 1024 );

    // these are all the error codes from the zookeeper.c source
    // file. we emit a suitable message on stderr and exit() with the
    // right sysexits code.
    switch ( r ) {
    case ZOK: // operation completed successfully
	break;
    case ZNONODE: // the parent node does not exist.
	cerr << "nodee: zk: Could not create " << path
	     << " (parent node deos not exist)" << endl;
	::exit( EX_UNAVAILABLE );
	break;
    case ZNODEEXISTS: // the node already exists
	cerr << "nodee: zk: Could not create " << path
	     << " (node already exists; is nodee running here?)" << endl;
	::exit( EX_UNAVAILABLE );
	break;
    case ZNOAUTH: // the client does not have permission.
	cerr << "nodee: zk: "
	     << "Authentication failed while connecting to zookeeper server "
	     << server << endl;
	::exit( EX_NOPERM );
	break;
    case ZNOCHILDRENFOREPHEMERALS: // cannot create children of ephemeral nodes.
	cerr << "nodee: zk: Could not create " << path
	     << " (parent node is ephemeral)" << endl;
	::exit( EX_UNAVAILABLE );
	break;
    case ZBADARGUMENTS: // invalid input parameters
	cerr << "nodee: zk: Could not create " << path
	     << " (zookeeper whines about bad arguments)" << endl;
	::exit( EX_DATAERR );
	break;
    case ZINVALIDSTATE: // zhandle state is either ZOO_SESSION_EXPIRED_STATE or ZOO_AUTH_FAILED_STATE
	cerr << "nodee: zk: Could not create " << path
	     << " (zookeeper worked itself into a bad state)" << endl;
	::exit( EX_UNAVAILABLE );
	break;
    case ZMARSHALLINGERROR: // failed to marshall a request; possibly, out of memory
	cerr << "nodee: zk: Could not create " << path
	     << " (zookeeper moans about marshalling problems)" << endl;
	::exit( EX_SOFTWARE );
	break;
    }

    // a lot of unreachable breaks there. ok.

    // at this point we've created the node and nodee can do its work.

    boost::thread( *this );
}


/*! This is run in a separate thread and is responsible for updating
    the ephemeral zk node with HostStatus content occasionally.
*/

void ZkClient::start()
{
    while( true ) {
	::sleep( 128 );

	string status = HostStatus();
	int r = zoo_set( zh, path.c_str(),
			 status.data(), status.length(),
			 -1 );
    }
}
