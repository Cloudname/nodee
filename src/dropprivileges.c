/* Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed. */

#include <unistd.h> // execvp
#include <sys/types.h> // uid_t
#include <stdlib.h> // atoi
#include <sysexits.h>

/* a small C program to drop root privileges and exec() its argument */

int main( int argc, char ** argv ) {
    if ( argc < 3 )
	exit( EX_USAGE );

    uid_t uid = atoi( argv[1] );
    gid_t gid = atoi( argv[2] );
    if ( !uid || !gid )
	exit( EX_USAGE );

    int i = 3;
    while ( i < argc ) {
	argv[i-3] = argv[i];
	i++;
    }
    argv[i-3] = 0;

    setregid( gid, gid );
    setreuid( uid, uid );
    if ( getuid() != uid || geteuid() != uid ||
	 getgid() != gid || getegid() != gid )
	exit( EX_NOPERM );

    execvp( argv[0], argv );
    exit( EX_NOINPUT );
}
