/*
$Id: ow_ds1410.c,v 1.72 2010/05/08 17:13:01 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
    email: palfille@earthlink.net
    Released under the GPL
    See the header file: ow.h for full attribution
    1wire/iButton system from Dallas Semiconductor
*/

#include <config.h>
#include "owfs_config.h"
#include "ow.h"

/* The parallel adapter has never worked for us, let's not pretend */

GOOD_OR_BAD DS1410_detect(struct connection_in *in)
{
	(void) in;
	return gbBAD;
}
