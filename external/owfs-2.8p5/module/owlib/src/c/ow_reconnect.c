/*
$Id: ow_reconnect.c,v 1.28 2010/09/24 21:30:28 alfille Exp $
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
#include "ow_counters.h"
#include "ow_connection.h"

/* Tests whether this bus (pn->selected_connection) has too many consecutive reset errors */
/* If so, the bus is closed and "reconnected" */
/* Reconnection usually just means reopening (detect) with the same initial name like ttyS0 */
/* USB is a special case, in gets reenumerated, so we look for similar DS2401 chip */
GOOD_OR_BAD TestConnection(const struct parsedname *pn)
{
	GOOD_OR_BAD ret = 0;
	struct connection_in *in = (pn == NO_PARSEDNAME) ? NO_CONNECTION : pn->selected_connection;

	//printf("Reconnect = %d / %d\n",selected_connection->reconnect_state,reconnect_error) ;
	// Test without a lock -- efficient
	if (in == NO_CONNECTION || in->reconnect_state < reconnect_error) {
		return gbGOOD;
	}
	// Lock the bus
	BUSLOCK(pn);
	// Test again
	if (in->reconnect_state >= reconnect_error) {
		// Add Statistics
		STAT_ADD1_BUS(e_bus_reconnects, in);

		// Close the bus (should leave enough reconnection information available)
		BUS_close(in);	// already locked

		// Call reconnection
		in->AnyDevices = anydevices_unknown ;
		if ( in->iroutines.reconnect != NO_RECONNECT_ROUTINE ) {
			// reconnect method exists
			ret = (in->iroutines.reconnect) (pn) ;	// call bus-specific reconnect
		} else {
			ret = BUS_detect(in) ;	// call initial opener
		}
		if ( BAD( ret ) ) {
			in->reconnect_state = reconnect_ok + 1 ;
			// delay to slow thrashing
			UT_delay(200);
		} else {
			in->reconnect_state = reconnect_ok;
		}
	}
	BUSUNLOCK(pn);

	if ( BAD( ret ) ) {
		LEVEL_DEFAULT("Failed to reconnect %s bus master!", in->adapter_name);
	} else {
		LEVEL_DEFAULT("%s bus master reconnected", in->adapter_name);
	}

	return ret;
}
