/*
$Id: ow_connection_in.h,v 1.11 2011/01/07 03:20:59 alfille Exp $
    OW -- One-Wire filesystem
    version 0.4 7/2/2003

    Written 2003 Paul H Alfille
        Fuse code based on "fusexmp" {GPL} by Miklos Szeredi, mszeredi@inf.bme.hu
        Serial code based on "xt" {GPL} by David Querbach, www.realtime.bc.ca
        in turn based on "miniterm" by Sven Goldt, goldt@math.tu.berlin.de
    GPL license
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

 */

#ifndef OW_CONNECTION_IN_H			/* tedious wrapper */
#define OW_CONNECTION_IN_H

/* struct connection_in (for each bus master) as part of ow_connection.h */

enum bus_speed {
	bus_speed_slow,
	bus_speed_overdrive,
};

enum bus_flex { bus_no_flex, bus_yes_flex };

// For forward references
struct connection_in;

/* -------------------------------------------- */
/* bUS-MASTER-specific routines ---------------- */
#include "ow_master.h"

#define CHANGED_USB_SPEED  0x001
#define CHANGED_USB_SLEW   0x002
#define CHANGED_USB_LOW    0x004
#define CHANGED_USB_OFFSET 0x008

//enum server_type { srv_unknown, srv_direct, srv_client, src_
/* Network connection structure */
enum bus_mode {
	bus_unknown = 0,
	bus_serial,
	bus_xport,
	bus_passive,
	bus_usb,
	bus_usb_monitor,
	bus_parallel,
	bus_server,
	bus_zero,
	bus_browse,
	bus_i2c,
	bus_ha7net,
	bus_ha5,
	bus_ha7e,
	bus_enet,
	bus_fake,
	bus_tester,
	bus_mock,
	bus_link,
	bus_elink,
	bus_etherweather,
	bus_bad,
	bus_w1,
	bus_w1_monitor,
	bus_xport_control,
};

enum adapter_type {
	adapter_DS9097 = 0,
	adapter_DS1410 = 1,
	adapter_DS9097U2 = 2,
	adapter_DS9097U = 3,
	adapter_LINK = 7,
	adapter_DS9490 = 8,
	adapter_tcp = 9,
	adapter_Bad = 10,
	adapter_LINK_10,
	adapter_LINK_11,
	adapter_LINK_12,
	adapter_LINK_13,
	adapter_LINK_14,
	adapter_LINK_other,
	adapter_LINK_E,
	adapter_DS2482_100,
	adapter_DS2482_800,
	adapter_HA7NET,
	adapter_HA5,
	adapter_HA7E,
	adapter_ENET,
	adapter_EtherWeather,
	adapter_fake,
	adapter_tester,
	adapter_mock,
	adapter_w1,
	adapter_w1_monitor,
	adapter_browse_monitor,
	adapter_xport,
	adapter_usb_monitor,
};

enum e_reconnect {
	reconnect_bad = -1,
	reconnect_ok = 0,
	reconnect_error = 2,
};

enum e_anydevices {
	anydevices_no = 0 ,
	anydevices_yes ,
	anydevices_unknown ,
};	

enum e_bus_stat {
	e_bus_reconnects,
	e_bus_reconnect_errors,
	e_bus_locks,
	e_bus_unlocks,
	e_bus_errors,
	e_bus_resets,
	e_bus_reset_errors,
	e_bus_short_errors,
	e_bus_program_errors,
	e_bus_pullup_errors,
	e_bus_timeouts,
	e_bus_read_errors,
	e_bus_write_errors,
	e_bus_detect_errors,
	e_bus_open_errors,
	e_bus_close_errors,
	e_bus_search_errors1,
	e_bus_search_errors2,
	e_bus_search_errors3,
	e_bus_status_errors,
	e_bus_select_errors,
	e_bus_try_overdrive,
	e_bus_failed_overdrive,
	e_bus_stat_last_marker
};

// Add serial/tcp/telnet abstraction
#include "ow_communication.h"

// flag to pn->selected_connection->branch.sn[0] to force select from root dir
#define BUSPATH_BAD	0xFF

struct connection_in {
	struct connection_in *next;
	INDEX_OR_ERROR index;
//	char *name;
//	FILE_DESCRIPTOR_OR_PERSISTENT file_descriptor;
	
	// Serial / tcp / telnet / i2c abstraction
	struct communication soc ;

	// Com port data:
//	speed_t baud; // baud rate in the form of B9600
//	struct termios oldSerialTio;    /*old serial port settings */
//	enum { flow_none, flow_soft, flow_hard, } flow_control ;
//	struct timeval timeout ; // for serial or tcp read
	
	// For adapters that maintain dir-at-once (or dirgulp):
	struct dirblob main;        /* main directory */
	struct dirblob alarm;       /* alarm directory */
	pthread_mutex_t bus_mutex;
	pthread_mutex_t dev_mutex;
	void *dev_db;				// dev-lock tree
	enum e_reconnect reconnect_state;
	struct timeval last_lock;	/* statistics */

	UINT bus_stat[e_bus_stat_last_marker];

	struct timeval bus_time;

	enum bus_mode busmode;
	struct interface_routines iroutines;
	enum adapter_type Adapter;
	char *adapter_name;
	enum e_anydevices AnyDevices;
	enum bus_speed speed;
	enum bus_flex flex ;
	int changed_bus_settings;
	int ds2404_found;
	int ProgramAvailable;
	size_t last_root_devs;
	struct buspath branch;		// Branch currently selected

	size_t bundling_length;

	union master_union master;
};

#define SOC( connection )  ( &( (connection)->soc) )

#define NO_CONNECTION NULL

extern struct inbound_control {
	int active ; // how many "bus" entries are currently in linked list
	int next_index ; // increasing sequence number
	struct connection_in * head ; // head of a linked list of "bus" entries
	my_rwlock_t monitor_lock; // allow monitor processes
	my_rwlock_t lock; // RW lock of linked list
	int next_fake ; // count of fake buses
	int next_tester ; // count tester buses
	int next_mock ; // count mock buses

	struct connection_in * w1_monitor ;
} Inbound_Control ; // Single global struct -- see ow_connect.c

#endif							/* OW_CONNECTION_IN_H */
