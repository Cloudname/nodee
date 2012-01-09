// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#define BOOST_TEST_MODULE HttpServer test

#include <boost/test/included/unit_test.hpp>

#include "httpserver.h"

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
    ofstream( "/tmp/vmstat" ) << "nr_free_pages 741357"
	"nr_inactive_anon 8197"
	"nr_active_anon 199740"
	"nr_inactive_file 570303"
	"nr_active_file 271414"
	"nr_unevictable 0"
	"nr_mlock 0"
	"nr_anon_pages 199483"
	"nr_mapped 48166"
	"nr_file_pages 850184"
	"nr_dirty 2"
	"nr_writeback 0"
	"nr_slab_reclaimable 83047"
	"nr_slab_unreclaimable 9907"
	"nr_page_table_pages 8825"
	"nr_kernel_stack 386"
	"nr_unstable 0"
	"nr_bounce 0"
	"nr_vmscan_write 0"
	"nr_writeback_temp 0"
	"nr_isolated_anon 0"
	"nr_isolated_file 0"
	"nr_shmem 8468"
	"nr_dirtied 2149225"
	"nr_written 1077377"
	"numa_hit 62949508"
	"numa_miss 0"
	"numa_foreign 0"
	"numa_interleave 7279"
	"numa_local 62949508"
	"numa_other 0"
	"nr_anon_transparent_hugepages 0"
	"nr_dirty_threshold 179101"
	"nr_dirty_background_threshold 89550"
	"pgpgin 1340803"
	"pgpgout 5048644"
	"pswpin 0"
	"pswpout 0"
	"pgalloc_dma 4"
	"pgalloc_dma32 108"
	"pgalloc_normal 67194697"
	"pgalloc_movable 0"
	"pgfree 67936762"
	"pgactivate 2113468"
	"pgdeactivate 0"
	"pgfault 38664271"
	"pgmajfault 7814"
	"pgrefill_dma 0"
	"pgrefill_dma32 0"
	"pgrefill_normal 0"
	"pgrefill_movable 0"
	"pgsteal_dma 0"
	"pgsteal_dma32 0"
	"pgsteal_normal 0"
	"pgsteal_movable 0"
	"pgscan_kswapd_dma 0"
	"pgscan_kswapd_dma32 0"
	"pgscan_kswapd_normal 0"
	"pgscan_kswapd_movable 0"
	"pgscan_direct_dma 0"
	"pgscan_direct_dma32 0"
	"pgscan_direct_normal 0"
	"pgscan_direct_movable 0"
	"zone_reclaim_failed 0"
	"pginodesteal 0"
	"slabs_scanned 0"
	"kswapd_steal 0"
	"kswapd_inodesteal 0"
	"kswapd_low_wmark_hit_quickly 0"
	"kswapd_high_wmark_hit_quickly 0"
	"kswapd_skip_congestion_wait 0"
	"pageoutrun 1"
	"allocstall 0"
	"pgrotated 0"
	"compact_blocks_moved 0"
	"compact_pages_moved 0"
	"compact_pagemigrate_failed 0"
	"compact_stall 0"
	"compact_fail 0"
	"compact_success 0"
	"htlb_buddy_alloc_success 0"
	"htlb_buddy_alloc_fail 0"
	"unevictable_pgs_culled 767"
	"unevictable_pgs_scanned 0"
	"unevictable_pgs_rescued 865"
	"unevictable_pgs_mlocked 865"
	"unevictable_pgs_munlocked 865"
	"unevictable_pgs_cleared 0"
	"unevictable_pgs_stranded 0"
	"unevictable_pgs_mlockfreed 0"
	"thp_fault_alloc 0"
	"thp_fault_fallback 0"
	"thp_collapse_alloc 0"
	"thp_collapse_alloc_failed 0"
	"thp_split 0";

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
