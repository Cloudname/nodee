// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <iostream>
#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE HttpServer test

#include <boost/test/included/unit_test.hpp>

#include "httpserver.h"

/*! \nodoc */

using namespace std;

BOOST_AUTO_TEST_CASE( HttpParsing )
{
    Init i;
    HttpServer x( 0, i );

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

BOOST_AUTO_TEST_CASE( HttpResponse )
{
    Init i;
    HttpServer x( 0, i );

    BOOST_CHECK_EQUAL( x.httpResponse( 100, "text/plain", "foo" ),
		       "HTTP/1.0 100 foo\r\n"
		       "Connection: close\r\n"
		       "Server: nodee\r\n"
		       "Content-Type: text/plain\r\n\r\n" );

    BOOST_CHECK_EQUAL( x.httpResponse( 200, "text/html", "OK" ),
		       "HTTP/1.0 200 OK\r\n"
		       "Connection: close\r\n"
		       "Server: nodee\r\n"
		       "Content-Type: text/html\r\n\r\n" );

    BOOST_CHECK_EQUAL( x.httpResponse( 404, "text/plain", "No!" ),
		       "HTTP/1.0 404 No!\r\n"
		       "Connection: close\r\n"
		       "Server: nodee\r\n"
		       "Content-Type: text/plain\r\n\r\n" );
}


#include "init.h"
#include "chorekeeper.h"

BOOST_AUTO_TEST_CASE( ReadProcVmstat )
{
    Init i;
    ChoreKeeper x( i );
    ofstream o( "/tmp/vmstat" );
    o << "nr_free_pages 741357\n"
	"nr_inactive_anon 8197\n"
	"nr_active_anon 199740\n"
	"nr_inactive_file 570303\n"
	"nr_active_file 271414\n"
	"nr_unevictable 0\n"
	"nr_mlock 0\n"
	"nr_anon_pages 199483\n"
	"nr_mapped 48166\n"
	"nr_file_pages 850184\n"
	"nr_dirty 2\n"
	"nr_writeback 0\n"
	"nr_slab_reclaimable 83047\n"
	"nr_slab_unreclaimable 9907\n"
	"nr_page_table_pages 8825\n"
	"nr_kernel_stack 386\n"
	"nr_unstable 0\n"
	"nr_bounce 0\n"
	"nr_vmscan_write 0\n"
	"nr_writeback_temp 0\n"
	"nr_isolated_anon 0\n"
	"nr_isolated_file 0\n"
	"nr_shmem 8468\n"
	"nr_dirtied 2149225\n"
	"nr_written 1077377\n"
	"numa_hit 62949508\n"
	"numa_miss 0\n"
	"numa_foreign 0\n"
	"numa_interleave 7279\n"
	"numa_local 62949508\n"
	"numa_other 0\n"
	"nr_anon_transparent_hugepages 0\n"
	"nr_dirty_threshold 179101\n"
	"nr_dirty_background_threshold 89550\n"
	"pgpgin 1340803\n"
	"pgpgout 5048644\n"
	"pswpin 0\n"
	"pswpout 0\n"
	"pgalloc_dma 4\n"
	"pgalloc_dma32 108\n"
	"pgalloc_normal 67194697\n"
	"pgalloc_movable 0\n"
	"pgfree 67936762\n"
	"pgactivate 2113468\n"
	"pgdeactivate 0\n"
	"pgfault 38664271\n"
	"pgmajfault 7814\n"
	"pgrefill_dma 0\n"
	"pgrefill_dma32 0\n"
	"pgrefill_normal 0\n"
	"pgrefill_movable 0\n"
	"pgsteal_dma 0\n"
	"pgsteal_dma32 0\n"
	"pgsteal_normal 0\n"
	"pgsteal_movable 0\n"
	"pgscan_kswapd_dma 0\n"
	"pgscan_kswapd_dma32 0\n"
	"pgscan_kswapd_normal 0\n"
	"pgscan_kswapd_movable 0\n"
	"pgscan_direct_dma 0\n"
	"pgscan_direct_dma32 0\n"
	"pgscan_direct_normal 0\n"
	"pgscan_direct_movable 0\n"
	"zone_reclaim_failed 0\n"
	"pginodesteal 0\n"
	"slabs_scanned 0\n"
	"kswapd_steal 0\n"
	"kswapd_inodesteal 0\n"
	"kswapd_low_wmark_hit_quickly 0\n"
	"kswapd_high_wmark_hit_quickly 0\n"
	"kswapd_skip_congestion_wait 0\n"
	"pageoutrun 1\n"
	"allocstall 0\n"
	"pgrotated 0\n"
	"compact_blocks_moved 0\n"
	"compact_pages_moved 0\n"
	"compact_pagemigrate_failed 0\n"
	"compact_stall 0\n"
	"compact_fail 0\n"
	"compact_success 0\n"
	"htlb_buddy_alloc_success 0\n"
	"htlb_buddy_alloc_fail 0\n"
	"unevictable_pgs_culled 767\n"
	"unevictable_pgs_scanned 0\n"
	"unevictable_pgs_rescued 865\n"
	"unevictable_pgs_mlocked 865\n"
	"unevictable_pgs_munlocked 865\n"
	"unevictable_pgs_cleared 0\n"
	"unevictable_pgs_stranded 0\n"
	"unevictable_pgs_mlockfreed 0\n"
	"thp_fault_alloc 0\n"
	"thp_fault_fallback 0\n"
	"thp_collapse_alloc 0\n"
	"thp_collapse_alloc_failed 0\n"
	"thp_split 0\n";

    int nfp = 1, pgmf = 2, pgpo = 3;
    x.readProcVmstat( "/tmp/vmstat", nfp, pgmf, pgpo );
    BOOST_CHECK_EQUAL( nfp, 741357 );
    BOOST_CHECK_EQUAL( pgmf, 7814 );
    BOOST_CHECK_EQUAL( pgpo, 5048644 );
}


BOOST_AUTO_TEST_CASE( ParseProcStat )
{
    Init i;
    ChoreKeeper x( i );

    RunningProcess r( x.parseProcStat( "1 (init) S 0 1 1 0 -1 4202752 15766 23427027 27 5186 129 133 929507 187524 20 0 1 0 3 24895488 603 18446744073709551615 1 1 0 0 0 0 0 4096 671180323 18446744073709551615 0 0 0 1 0 0 4 0 0" ) );
    BOOST_CHECK_EQUAL( r.pid, 1 );
    BOOST_CHECK_EQUAL( r.ppid, 0 );
    BOOST_CHECK_EQUAL( r.rss, 603 );
    BOOST_CHECK_EQUAL( r.majflt, 27 + 5186 );

    // next, with a trailing newline
    r = x.parseProcStat( "14654 (zsh) S 13820 14654 14654 34818 14658 4202496 1470 793 0 0 6 2 0 0 20 0 1 0 20880374 39575552 731 18446744073709551615 4194304 4849052 140737478282368 140737478280640 140637896447738 0 2 3686404 134291459 18446744071579325599 0 0 17 0 0 0 0 0 0\r" );
    BOOST_CHECK_EQUAL( r.pid, 14654 );
    BOOST_CHECK_EQUAL( r.ppid, 13820 );
    BOOST_CHECK_EQUAL( r.rss, 731 );
    BOOST_CHECK_EQUAL( r.majflt, 0 );
}


static void makeFakeProc()
{
    boost::filesystem::create_directory( "/tmp/uglehack" );
    boost::filesystem::create_directory( "/tmp/uglehack/1" );
    boost::filesystem::create_directory( "/tmp/uglehack/42" );
    boost::filesystem::create_directory( "/tmp/uglehack/69" );
    boost::filesystem::create_directory( "/tmp/uglehack/100" );
    boost::filesystem::create_directory( "/tmp/uglehack/101" );
    boost::filesystem::create_directory( "/tmp/uglehack/200" );
    boost::filesystem::create_directory( "/tmp/uglehack/201" );
    boost::filesystem::create_directory( "/tmp/uglehack/202" );

    ofstream f1( "/tmp/uglehack/1/stat" );
    f1 << "1 (init) S 0 0 0 0 0 0 0 0 27 5186 0 0 0 0 0 0 0 0 0 0 603 0" << endl;
    ofstream f1b( "/tmp/uglehack/1/status" );
    f1b << "Name: init" << endl
	<< "Uid:	0	0	0	0" << endl
	<< "Gid:	0	0	0	0" << endl;
    ofstream f42( "/tmp/uglehack/42/stat" );
    f42 << "42 (nodee) S 0 0 0 0 0 0 0 0 30 30 0 0 0 0 0 0 0 0 0 0 1000 0" << endl;
    ofstream f42b( "/tmp/uglehack/42/status" );
    f42b << "Name: nodee" << endl
	 << "Uid:	0	0	0	0" << endl
	 << "Gid:	0	0	0	0" << endl;
    ofstream f69( "/tmp/uglehack/69/stat" );
    f69 << "69 (sshd) S 0 0 0 0 0 0 0 0 40 1 0   0 0 0 0 0 0 0 0 100 0" << endl;
    ofstream f69b( "/tmp/uglehack/69/status" );
    f69b << "Name: sshd" << endl
	 << "Uid:	0	0	0	0" << endl
	 << "Gid:	0	0	0	0" << endl;
    ofstream f100( "/tmp/uglehack/100/stat" );
    f100 << "100 (mg1) S 0 0 0 0 0 0 0 0 1000 0 0 0 0 0 0 0 0 0 0 0 1532 0" << endl;
    ofstream f100b( "/tmp/uglehack/100/status" );
    f100b << "Name: mg1" << endl
	  << "Uid:	2000	2000	2000	2000" << endl
	  << "Gid:	2000	2000	2000	2000" << endl;
    ofstream f101( "/tmp/uglehack/101/stat" );
    f101 << "101 (mg1c) S 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 1432 0" << endl;
    ofstream f101b( "/tmp/uglehack/101/status" );
    f101b << "Name: mg1c" << endl
	  << "Uid:	2000	2000	2000	2000" << endl
	  << "Gid:	2000	2000	2000	2000" << endl;
    ofstream f200( "/tmp/uglehack/200/stat" );
    f200 << "200 (mg2) S 0 0 0 0 0 0 0 0 4576 0 0 0 0 0 0 0 0 0 0 0 3883 0" << endl;
    ofstream f200b( "/tmp/uglehack/200/status" );
    f200b << "Name: mg2" << endl
	  << "Uid:	2001	2001	2001	2001" << endl
	  << "Gid:	2002	2002	2002	2002" << endl;
    ofstream f201( "/tmp/uglehack/201/stat" );
    f201 << "201 (mg2c) S 0 0 0 0 0 0 0 0 69 0 0 0 0 0 0 0 0 0 0 0 4231235 0" << endl;
    ofstream f201b( "/tmp/uglehack/201/status" );
    f201b << "Name: mg2c" << endl
	  << "Uid:	2001	2001	2001	2001" << endl
	  << "Gid:	2002	2002	2002	2002" << endl;
    ofstream f202( "/tmp/uglehack/202/stat" );
    f202 << "202 (mg2cc) S 0 0 0 0 0 0 0 0 42 0 0 0 0 0 0 0 0 0 0 0 476238 0" << endl;
    ofstream f202b( "/tmp/uglehack/202/status" );
    f202b << "Name: mg2cc" << endl
	  << "Uid:	2001	2001	2001	2001" << endl
	  << "Gid:	2002	2002	2002	2002" << endl;
}

BOOST_AUTO_TEST_CASE( ScanProcesses )
{
    Init i;
    ChoreKeeper x( i );

    // we set up a tree with eight processes: three in one managed
    // service, two in another, nodee itself, and two others.

    // init: 1 should be ignored
    // nodee: 42 should be ignored
    // sshd: 69 should be ignored
    // mg1: 100 should be counted
    // mg1c: 101 should be assigned to mg1
    // mg2: 200 should be counted
    // mg2c: 201 should be assigned to mg2
    // mg2cc: 202 should be assigned to mg2

    Process mg1;
    mg1.fakefork( 100 );
    i.manage( mg1 );

    Process mg2;
    mg2.fakefork( 200 );
    i.manage( mg2 );

    makeFakeProc();

    x.scanProcesses( "/tmp/uglehack", 42 );

    // should have observed 100, with majflt 1000+0+1+0 and rss 1532+1432,
    // and 200, with majflt 4576+0+69+0+42+0 and rss 3883+4231235+476238.

    BOOST_CHECK( x.biggest() == mg2 );
    BOOST_CHECK( x.thrashingMost() == mg2 );
    BOOST_CHECK( !x.leastValuable().valid() );
    // peak and expected are both zero, so...
    BOOST_CHECK( x.furthestOverPeak() == mg2 );
    BOOST_CHECK( x.furthestOverExpected() == mg2 );
}


#include "service.h"


BOOST_AUTO_TEST_CASE( ServiceList )
{
    Init i;

    Process p1;
    p1.fakefork( 100 );
    p1.setCurrentRss( 100 );
    p1.setPageFaults( 29 );

    i.manage( p1 );

    BOOST_CHECK_EQUAL( Service::list( i ), "{\n"
		       "    \"services\":\n"
		       "    {\n"
		       "        \"100\":\n"
		       "        {\n"
		       "            \"value\": \"0\",\n"
		       "            \"rss\": \"100\",\n"
		       "            \"recentfaults\": \"29\"\n"
		       "        }\n"
		       "    }\n"
		       "}\n" );
}


#include "uid.h"

BOOST_AUTO_TEST_CASE( ReadEtcPasswd )
{
    ofstream o( "/tmp/passwd" );
    o << "root:x:0:0:root:/root:/bin/zsh\n"
	"daemon:x:1:1:daemon:/usr/sbin:/bin/sh\n"
	"bin:x:2:2:bin:/bin:/bin/sh\n"
	"sys:x:3:3:sys:/dev:/bin/sh\n"
	"sync:x:4:65534:sync:/bin:/bin/sync\n"
	"games:x:5:60:games:/usr/games:/bin/sh\n"
	"man:x:6:12:man:/var/cache/man:/bin/sh\n"
	"lp:x:7:7:lp:/var/spool/lpd:/bin/sh\n"
	"lp2:x:2000:2000:lp:/var/spool/lpd:/bin/sh\n"
	"lp3:x:2002:7:lp:/var/spool/lpd:/bin/sh\n";
    o.flush();
    set<int> p = inPasswd( false, "/tmp/passwd" );
    BOOST_CHECK( p.find( 0 ) != p.end() );
    BOOST_CHECK( p.find( 5 ) != p.end() );
    BOOST_CHECK( p.find( 7 ) != p.end() );
    BOOST_CHECK( p.find( 2000 ) != p.end() );
    BOOST_CHECK( p.find( 2002 ) != p.end() );
    BOOST_CHECK( p.find( 2001 ) == p.end() );
    BOOST_CHECK( p.find( 2003 ) == p.end() );
    BOOST_CHECK( p.find( 20003 ) == p.end() );
    BOOST_CHECK( p.find( 60000 ) == p.end() );

    set<int> g = inPasswd( true, "/tmp/passwd" );
    BOOST_CHECK( g.find( 0 ) != g.end() );
    BOOST_CHECK( g.find( 60 ) != g.end() );
    BOOST_CHECK( g.find( 7 ) != g.end() );
    BOOST_CHECK( g.find( 2000 ) != g.end() );
    BOOST_CHECK( g.find( 2002 ) == g.end() );
    BOOST_CHECK( g.find( 2001 ) == g.end() );
    BOOST_CHECK( g.find( 2003 ) == g.end() );
}


BOOST_AUTO_TEST_CASE( ReadProcUids )
{
    makeFakeProc();
    set<int> p = inProc( false, "/tmp/uglehack" );
    BOOST_CHECK( p.find( 0 ) != p.end() );
    BOOST_CHECK( p.find( 5 ) == p.end() );
    BOOST_CHECK( p.find( 2000 ) != p.end() );
    BOOST_CHECK( p.find( 2001 ) != p.end() );
    BOOST_CHECK( p.find( 2002 ) == p.end() );
    BOOST_CHECK( p.find( 2003 ) == p.end() );
    BOOST_CHECK( p.find( 20003 ) == p.end() );
    BOOST_CHECK( p.find( 60000 ) == p.end() );

    set<int> g = inProc( true, "/tmp/uglehack" );
    BOOST_CHECK( g.find( 0 ) != g.end() );
    BOOST_CHECK( g.find( 5 ) == g.end() );
    BOOST_CHECK( g.find( 2000 ) != g.end() );
    BOOST_CHECK( g.find( 2002 ) != g.end() );
    BOOST_CHECK( g.find( 2001 ) == g.end() );
    BOOST_CHECK( g.find( 2003 ) == g.end() );
    BOOST_CHECK( g.find( 20003 ) == g.end() );
    BOOST_CHECK( g.find( 60000 ) == g.end() );
}


#include "hoststatus.h"

BOOST_AUTO_TEST_CASE( CpuCounter )
{
    ofstream o( "/tmp/cpuinfo" );
    o << "processor	: 0\n"
	"vendor_id	: AuthenticAMD\n"
	"cpu family	: 20\n"
	"cpu MHz		: 800.000\n"
	"cpuid level	: 6\n"
	"wp		: yes\n"
	"\n"
	"processor	: 1\n"
	"vendor_id	: AuthenticAMD\n"
	"cpu family	: 20\n";
    o.flush();
    BOOST_CHECK_EQUAL( HostStatus::cores( "/tmp/cpuinfo" ), 2 );
}

BOOST_AUTO_TEST_CASE( MemSizeEstimator )
{
    ofstream m( "/tmp/meminfo" );
    m << "MemTotal:        7787796 kB\n"
	"MemFree:          851632 kB\n"
	"Buffers:          322116 kB\n"
	"Cached:          5303584 kB\n"
	"SwapCached:            0 kB\n"
	"Active:          1825320 kB\n"
	"Inactive:        4744892 kB\n"
	"Active(anon):     699736 kB\n"
	"Inactive(anon):   279968 kB\n"
	"Active(file):    1125584 kB\n"
	"Inactive(file):  4464924 kB\n"
	"Unevictable:           0 kB\n"
	"Mlocked:               0 kB\n"
	"SwapTotal:       7974908 kB\n"
	"SwapFree:        7974800 kB\n"
	"HugePages_Total:       0\n"
	"HugePages_Free:        0\n"
	"HugePages_Rsvd:        0\n"
	"HugePages_Surp:        0\n"
	"Hugepagesize:       2048 kB\n";
    m.flush();

    int t, a;
    HostStatus::readProcMeminfo( "/tmp/meminfo", t, a );

    BOOST_CHECK_EQUAL( t, 7787796 );
    BOOST_CHECK_EQUAL( a, 851632 + 4744892 );
}


#include "port.h"

BOOST_AUTO_TEST_CASE( FreePorts ) {
    ofstream ptt( "/tmp/tcp" );
    ptt << "sl  local_address rem_address   st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode\n"
	"   0: 00000000:006F 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 6057 1 0000000000000000 300 0 0 2 -1                      \n"
	"   1: 00000000:0016 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 8268 1 0000000000000000 300 0 0 2 -1                      \n"
	"   2: 0100007F:0277 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 12864707 1 0000000000000000 300 0 0 2 -1                  \n"
	"   3: 00000000:C2F8 00000000:0000 0A 00000000:00000000 00:00000000 00000000   115        0 8340 1 0000000000000000 300 0 0 2 -1                      \n"
	"   4: 0100007F:0019 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 8683 1 0000000000000000 300 0 0 2 -1                      \n"
	"   5: 00000000:DE0C 00000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 437528 1 0000000000000000 300 0 0 2 -1                    \n"
	"  sl  local_address                         remote_address                        st tx_queue rx_queue tr tm->when retrnsmt   uid  timeout inode\n"
	"   0: 00000000000000000000000000000000:006F 00000000000000000000000000000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 6064 1 0000000000000000 300 0 0 2 -1\n"
	"   1: 00000000000000000000000000000000:8B15 00000000000000000000000000000000:0000 0A 00000000:00000000 00:00000000 00000000   115        0 8348 1 0000000000000000 300 0 0 2 -1\n"
	"   2: 00000000000000000000000000000000:0016 00000000000000000000000000000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 8270 1 0000000000000000 300 0 0 2 -1\n"
	"   3: 00000000000000000000000001000000:0277 00000000000000000000000000000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 12864706 1 0000000000000000 300 0 0 2 -1\n"
	"   4: 00000000000000000000000000000000:B1C8 00000000000000000000000000000000:0000 0A 00000000:00000000 00:00000000 00000000     0        0 437530 1 0000000000000000 300 0 0 2 -1\n"
	"   5: 884D012001000C10FF2E01026ABA36FE:8767 884D012000000C100000000001000000:0016 01 00000000:00000000 02:000A7538 00000000  1001        0 7714047 2 0000000000000000 24 4 28 10 -1\n";
    ptt.flush();

    set<int> taken = Port::busy( "/tmp/tcp" );

    BOOST_CHECK( taken.find( 45512 ) != taken.end() );
    BOOST_CHECK( taken.find( 111 ) != taken.end() );
    BOOST_CHECK( taken.find( 631 ) != taken.end() );
    BOOST_CHECK( taken.find( 0x8767 ) != taken.end() );
    BOOST_CHECK( taken.find( 110 ) == taken.end() );
    BOOST_CHECK( taken.find( 112 ) == taken.end() );
}


#include "artifact.h"
#include "conf.h"


BOOST_AUTO_TEST_CASE( ArtifactLister )
{
    makeFakeProc();
    Conf::artefactdir = "/tmp/uglehack/1";

    BOOST_CHECK_EQUAL( Artifact::list(),
		       "{\n"
		       "    \"1\": \"stat\",\n"
		       "    \"2\": \"status\"\n"
		       "}\n" );
}


#include "serverspec.h"

// I feel a need to record the following error message, which g++ gave me
// when I stupidly attempted to use pt::const_assoc_iterator as if it were
// the std kind of iterator

// serverspec.cpp:349:23: error: no match for ‘operator=’ in ‘r.std::map<_Key, _Tp, _Compare, _Alloc>::operator[] [with _Key = std::basic_string<char>, _Tp = std::basic_string<char>, _Compare = std::less<std::basic_string<char> >, _Alloc = std::allocator<std::pair<const std::basic_string<char>, std::basic_string<char> > >, std::map<_Key, _Tp, _Compare, _Alloc>::mapped_type = std::basic_string<char>, std::map<_Key, _Tp, _Compare, _Alloc>::key_type = std::basic_string<char>]((* &((boost::iterator_facade<boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> >::const_assoc_iterator, std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >, boost::bidirectional_traversal_tag, const std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >&, long int>*)(& i))->boost::iterator_facade<I, V, TC, R, D>::operator-> [with Derived = boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> >::const_assoc_iterator, Value = std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >, CategoryOrTraversal = boost::bidirectional_traversal_tag, Reference = const std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >&, Difference = long int, boost::iterator_facade<I, V, TC, R, D>::pointer = const std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >*]()->std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >::first)) = ((boost::iterator_facade<boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> >::const_assoc_iterator, std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >, boost::bidirectional_traversal_tag, const std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >&, long int>*)(& i))->boost::iterator_facade<I, V, TC, R, D>::operator-> [with Derived = boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> >::const_assoc_iterator, Value = std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >, CategoryOrTraversal = boost::bidirectional_traversal_tag, Reference = const std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >&, Difference = long int, boost::iterator_facade<I, V, TC, R, D>::pointer = const std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >*]()->std::pair<const std::basic_string<char>, boost::property_tree::basic_ptree<std::basic_string<char>, std::basic_string<char> > >::second’

// it did make me pause for a moment or two, I don't mind admitting

BOOST_AUTO_TEST_CASE( ServerOptions )
{
    Init i;
    ServerSpec s = ServerSpec::parseJson(
	"{"
	"  \"coordinate\" : \"1.idee-prod.ideeuser.ie\","
	"  \"artifact\" : \"com.telenor:id-server:1.4.2\","
	"  \"filename\" : \"id-server-1.4.2-shaded.jar\","
	"  \"url\" : \"http://haw-lin.com\","
	"  \"options\" : {"
	"    \"--someoption\" : \"some value\","
	"    \"--anotheroption\" : \"more config\""
	"  },"
	"  \"restart\" : {"
	"    \"period\" : 120,"
	"    \"maxrestarts\" : 10,"
	"    \"enabled\" : true"
	"  }"
	"}", i
	);
    BOOST_CHECK( s.valid() );
    map<string,string> o = s.startupOptions();
    BOOST_CHECK_EQUAL( o["--someoption"], "some value" );
    BOOST_CHECK_EQUAL( o["--anotheroption"], "more config" );
}
