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
string Conf::scriptdir;
string Conf::basedir;
string Conf::workdir;
string Conf::artefactdir;
string Conf::zk;


/*! Writes default values into the configuration values. The default
    values are documented (only) in Nodee's user documentation, not
    here.
*/

void Conf::setDefaults()
{
}
