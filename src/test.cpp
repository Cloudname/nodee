// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include <iostream>
#include <boost/filesystem.hpp>

#define BOOST_TEST_MODULE HttpServer test

#include <boost/test/included/unit_test.hpp>

#include "httpserver.h"

using namespace std;

BOOST_AUTO_TEST_CASE( HttpParsing )
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

BOOST_AUTO_TEST_CASE( HttpResponse )
{
    HttpServer x( 0 );

    BOOST_CHECK_EQUAL( x.httpResponse( 100, "text/plain", "foo" ),
		       "100 foo\r\n"
		       "Connection: close\r\n"
		       "Server: nodee\r\n"
		       "Content-Type: text/plain\r\n\r\n" );

    BOOST_CHECK_EQUAL( x.httpResponse( 200, "text/html", "OK" ),
		       "200 OK\r\n"
		       "Connection: close\r\n"
		       "Server: nodee\r\n"
		       "Content-Type: text/html\r\n\r\n" );

    BOOST_CHECK_EQUAL( x.httpResponse( 404, "text/plain", "No!" ),
		       "404 No!\r\n"
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
    ofstream f42( "/tmp/uglehack/42/stat" );
    f42 << "42 (nodee) S 0 0 0 0 0 0 0 0 30 30 0 0 0 0 0 0 0 0 0 0 1000 0" << endl;
    ofstream f69( "/tmp/uglehack/69/stat" );
    f69 << "69 (sshd) S 0 0 0 0 0 0 0 0 40 1 0   0 0 0 0 0 0 0 0 100 0" << endl;
    ofstream f100( "/tmp/uglehack/100/stat" );
    f100 << "100 (mg1) S 0 0 0 0 0 0 0 0 1000 0 0 0 0 0 0 0 0 0 0 0 1532 0" << endl;
    ofstream f101( "/tmp/uglehack/101/stat" );
    f101 << "101 (mg1c) S 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 1432 0" << endl;
    ofstream f200( "/tmp/uglehack/200/stat" );
    f200 << "200 (mg2) S 0 0 0 0 0 0 0 0 4576 0 0 0 0 0 0 0 0 0 0 0 3883 0" << endl;
    ofstream f201( "/tmp/uglehack/201/stat" );
    f201 << "201 (mg2c) S 0 0 0 0 0 0 0 0 69 0 0 0 0 0 0 0 0 0 0 0 4231235 0" << endl;
    ofstream f202( "/tmp/uglehack/202/stat" );
    f202 << "202 (mg2cc) S 0 0 0 0 0 0 0 0 42 0 0 0 0 0 0 0 0 0 0 0 476238 0" << endl;

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
