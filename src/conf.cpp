// Copyright Arnt Gulbrandsen <arnt@gulbrandsen.priv.no>; BSD-licensed.

#include "conf.h"



/*! \class Conf conf.h
  
    The Conf class is a wrapper for some global variables and two
    function to access one variable and return it in the desirable
    format.

    Globals and singletons are highly undesirable, so please don't
    turn the class into more than it is.
*/


map<string,string> Conf::depots;
string Conf::basedir;
string Conf::workdir;
string Conf::artefactdir;
string Conf::zk;
