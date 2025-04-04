/*
$Id: ow_1820.h,v 1.8 2009/12/12 02:10:16 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
	email: palfille@earthlink.net
	Released under the GPL
	See the header file: ow.h for full attribution
	1wire/iButton system from Dallas Semiconductor
    
    LICENSE (As of version 2.5p4 2-Oct-2006)
    owlib: LGPL v2
    owfs, owhttpd, owftpd, owserver: GPL v2
    owshell(owdir owread owwrite owpresent): GPL v2
    owcapi (libowcapi): LGPL v2
    owperl: GPL v2
    owtcl: GPL v2 or later at your discretion
    owphp: GPL v2
    owpython: GPL v2
    owsim.tcl: GPL v2
    where GPL v2 is the "Gnu General License version 2"
    and "LGPL v2" is the "Lesser Gnu General License version 2"

*/

#ifndef OW_1820_H
#define OW_1820_H

#ifndef OWFS_CONFIG_H
#error Please make sure owfs_config.h is included *before* this header file
#endif
#include "ow_standard.h"

/* -------- Structures ---------- */
DeviceHeader(DS18S20);
DeviceHeader(DS18B20);
DeviceHeader(DS1822);
DeviceHeader(DS1825);
DeviceHeader(DS28EA00);

#endif
