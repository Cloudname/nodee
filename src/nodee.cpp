// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <sysexits.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "httplistener.h"
#include "chorekeeper.h"
#include "zkclient.h"
#include "init.h"
#include "conf.h"
#include "log.h"

#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem.hpp>

#include <boost/algorithm/string.hpp>

#if !defined(CONFFILE)
#define CONFFILE "/etc/nodee/config"
#endif


using namespace boost::program_options;


/*! \nodoc */


int main( int argc, char ** argv )
{
    Conf::setDefaults();

    int port;
    vector<string> depots;
    string cf( CONFFILE );

    options_description cli( "Command-line options" );
    cli.add_options()
	( "help", "produce help message" )
	( "config,C", value<string>(&cf)->default_value( CONFFILE ),
	  "specify configuration file" )
	( "show-config", "print configuration at startup" )
	( "version,V", "show easter egg" );

    options_description conf( "Configuration file (and command-line) options" );
    conf.add_options()
	( "port,P", value<int>( &port )->default_value( 40 ),
	  "set nodee TCP port" )
	( "depot", value<vector<string> >( &depots )->composing(),
	  "add artefact depot (e.g. example=http://artefactory.example.com/)" )
	( "dir",
	  value<string>( &Conf::basedir )->default_value( "/usr/local/nodee" ),
	  "specify base directory" )
	( "work-dir",
	  value<string>( &Conf::workdir )->default_value( "work" ),
	  "specify work directory, relative to the base directory" )
	( "artefact-dir",
	  value<string>( &Conf::artefactdir )->default_value( "artefacts" ),
	  "specify where to store artefacts, relative to the base directory" )
	( "script-dir",
	  value<string>( &Conf::scriptdir )->default_value( "/etc/nodee/scripts" ),
	  "specify where the download and install scripts live" )
	( "zookeeper", value<string>( &Conf::zk ),
	  "zookeeper location (e.g. FIXME)" );

    variables_map vm;

    cli.add( conf );

    try {
	store( parse_command_line( argc, argv, cli ), vm );
	store( parse_environment( cli, "NODEE_" ), vm );
	ifstream cfs( cf.c_str() );
	store( parse_config_file( cfs, conf, false ), vm );
    } catch ( unknown_option e ) {
	cerr << "nodee: Unknown option "
	     << e.get_option_name()
	     << endl;
	::exit( EX_USAGE );
    } catch ( invalid_syntax e ) {
	cerr << "nodee: Syntax error while parsing options."
	     << endl; // boost::po has $%#@$@#$ error reporting
	::exit( EX_USAGE );
    } catch ( error e ) {
	cerr << "nodee: Option parser complains about you."
	     << endl
	     << "       This will be reported to your superior."
	     << endl;
	::exit( EX_USAGE );
    } catch ( ... ) {
	cerr << "nodee: Unknown error while parsing command line "
	        "or configuration file. Sorry."
	     << endl;
	::exit( EX_DATAERR );
    }

    notify( vm );

    bool dumpdepots = false;
    if ( depots.size() ) {
	int i = depots.size();
	while ( i ) {
	    --i;

	    std::vector<std::string> tmp;
	    boost::split( tmp, depots[i], boost::is_any_of("=") );
	    if ( tmp.size() != 2 ) {
		cerr << "nodee: Syntax error in artefact depot: "
		     << depots[i] << endl;
		exit( 1 );
	    }
	    if ( !Conf::depots[tmp[0]].empty() &&
		 Conf::depots[tmp[0]] != tmp[1] ) {
		cout << "nodee: Ignoring depot "
		     << Conf::depots[tmp[0]]
		     << " for group "
		     << tmp[0]
		     << endl;
		dumpdepots = true;
	    }
	    Conf::depots[tmp[0]] = tmp[1];
	}
    }

    bool fail = false;

    if ( !boost::filesystem::is_directory( Conf::basedir ) ) {
	cerr << "Nodee: Cannot start up, base directory does not exist"
	     << endl;
	fail = true;
    }
    if ( !boost::filesystem::is_directory( Conf::basedir + "/" + Conf::workdir  ) ) {
	cerr << "Nodee: Cannot start up, work directory does not exist"
	     << endl;
	fail = true;
    }
    if ( !boost::filesystem::is_directory( Conf::basedir + "/" + Conf::artefactdir ) ) {
	cerr << "Nodee: Cannot start up, artefact directory does not exist"
	     << endl;
	fail = true;
    }


    if ( fail || vm.count( "show-config" ) ) {
	dumpdepots = true;
	cout << "nodee: scriptdir is '" << Conf::scriptdir << "'" << endl
	     << "nodee: basedir is '" << Conf::basedir << "'" <<  endl
	     << "nodee: workdir is '" << Conf::basedir << '/'
	     << Conf::workdir << "'" << endl
	     << "nodee: artefactdir is '" << Conf::basedir << '/'
	     << Conf::artefactdir <<  "'" << endl
	     << "nodee: zk is '" << Conf::zk <<  "'" << endl;
    }

    if ( dumpdepots ) {
	map<string,string>::iterator i = Conf::depots.begin();
	while ( i != Conf::depots.end() ) {
	    cout << "nodee: Group "
		 << i->first
		 << " uses depot "
		 << i->second
		 << endl;
	    ++i;
	}
    }

    if ( vm.count( "version" ) ) {
	cout << "nodee: Version " << VERSION
	     << ", built " DATE
	     << endl;
	exit( 0 );
    }

    if ( vm.count( "help" ) ) {
	cout << cli << endl;
	exit( 0 );
    }

    if ( fail )
	::exit( EX_USAGE );

    if ( geteuid() )
	info << "Nodee: Running as non-root. "
	        "All services will use the same UID as nodee."
	     << endl;

    ZkClient zk( Conf::zk );

    Init i;

    HttpListener h6( HttpListener::V6, port, i );
    HttpListener h4( HttpListener::V4, port, i );

    if ( !h6.valid() && !h4.valid() ) {
	cerr << "nodee: Unable to listen to port "
	     << port
	     << " on either IPv4 or v6, exiting"
	     << endl;
	exit( 1 );
    }

    ChoreKeeper k( i );
    k.start();
}

/*! \chapter index
    \introduces ChoreKeeper HostStatus HttpServer HttpListener Init Process ServerSpec ZkClient

    Nodee uses eight main classes, plus some adjuncts.

    Init is the mothership. It watches over the running processes.
    When a new service is started, that always results in at least one
    new Process object, which generally does nothing. The process does
    something and Init watches over it. Init has its own thread.

    Process merely records information about its process... and
    contains a static function, Process::launch(), to launch new
    processes.

    ServerSpec is what Process uses to remember things about the
    service. ServerSpec does the main json work, decoding and encoding
    things for the HTTP clients.

    HttpServer is the process that mostly calls Process::launch(). It
    serves HTTP API requests to start/stop services and calls other
    parts of nodee to do the work.

    HttpListener helps HttpServer and creates new HttpServer objects
    as needed; HttpListener is one of nodee's long-running threads.

    HostStatus and ChoreKeeper look at the host nodee runs
    on. HostStatus merely tells all comers about the host ("it has
    umpty-foo gigabytes of RAM"), ChoreKeeper actually does some
    work. If the host runs out of RAM, ChoreKeeper will discover that,
    find out which service is most likely to make the difference
    between useful service and thrashing, and kill that.

    Finally, ZkClient is used by nodee to maintain state information
    about the host in zookeeper. ZkClient uses no less than two
    long-running threads.

    The function main() does most of the setup.

    Utility code for nodee comes from boost (1.40 or later) and the
    C++ standard library (the old one, not TR1).

    boost::filesystem changed API between 1.40 and 1.42, which causes
    some scattered hackage around the nodee source. I (Arnt) think
    having these turds is preferable to requiring a newer version
    boost than what Ubuntu LTS offers.
*/
