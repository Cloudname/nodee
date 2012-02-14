// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "log.h'

ostream info;
ostream debug;


/*! \class Log log.h

  Log is Nodee's logging helper; a set of ostreams to be used for logging.
  
  One can write to the debug logger merely by using the ostream
  debug. At the moment that's cerr, but it can be something else.
  
  An implementation note: By playing around with two ostream classes,
  it's possible to gather the output from composite operations such as
  debug << a << b and send them to the log server as one string.
  Slightly tricky, but doable, so I think this design is right.
*/


/*! This static function initialises the log system to write
    everything to stderr.
*/

void Log::setupStderr()
{
    info = cerr;
    debug = cerr;
}
