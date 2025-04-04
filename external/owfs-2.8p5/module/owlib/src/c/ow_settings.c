/*
$Id: ow_settings.c,v 1.46 2011/01/01 15:03:42 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
    email: palfille@earthlink.net
    Released under the GPL
    See the header file: ow.h for full attribution
    1wire/iButton system from Dallas Semiconductor
*/

/* Settings are a pseudo-device -- used to dynamicalls change timeouts and the like
 * these settings can also be changed at the command line
 * Except for performance, none of these settings has security implications
 * */

#include <config.h>
#include "owfs_config.h"
#include "ow_settings.h"

/* ------- Prototypes ----------- */
/* Statistics reporting */
READ_FUNCTION(FS_r_timeout);
WRITE_FUNCTION(FS_w_timeout);
READ_FUNCTION(FS_r_TS);
WRITE_FUNCTION(FS_w_TS);
READ_FUNCTION(FS_r_PS);
WRITE_FUNCTION(FS_w_PS);

/* -------- Structures ---------- */

struct filetype set_timeout[] = {
	{"volatile", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_volatile},},
	{"stable", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_stable},},
	{"directory", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_directory},},
	{"presence", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_presence},},
	{"serial", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_serial},},
	{"usb", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_usb},},
	{"network", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_network},},
	{"server", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_server},},
	{"ftp", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_ftp},},
	{"ha7", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_ha7},},
	{"w1", PROPERTY_LENGTH_UNSIGNED, NON_AGGREGATE, ft_unsigned, fc_static, FS_r_timeout, FS_w_timeout, VISIBLE, {v:&Globals.timeout_w1},},
}

;
struct device d_set_timeout = { "timeout", "timeout", ePN_settings, COUNT_OF_FILETYPES(set_timeout),
	set_timeout, NO_GENERIC_READ, NO_GENERIC_WRITE
};
struct filetype set_units[] = {
 	{"temperature_scale", 1, NON_AGGREGATE, ft_ascii, fc_static, FS_r_TS, FS_w_TS, VISIBLE, NO_FILETYPE_DATA,},
 	{"pressure_scale", 12, NON_AGGREGATE, ft_ascii, fc_static, FS_r_PS, FS_w_PS, VISIBLE, NO_FILETYPE_DATA,},
}

;
struct device d_set_units = { "units", "units", ePN_settings, COUNT_OF_FILETYPES(set_units),
	set_units, NO_GENERIC_READ, NO_GENERIC_WRITE
};

/* ------- Functions ------------ */

static ZERO_OR_ERROR FS_r_timeout(struct one_wire_query *owq)
{
	CACHE_RLOCK;
	OWQ_U(owq) = ((UINT *) OWQ_pn(owq).selected_filetype->data.v)[0];
	CACHE_RUNLOCK;
	return 0;
}

static ZERO_OR_ERROR FS_w_timeout(struct one_wire_query *owq)
{
	int previous;
	struct parsedname *pn = PN(owq);
	CACHE_WLOCK;
	//printf("FS_w_timeout!!!\n");
	previous = ((UINT *) pn->selected_filetype->data.v)[0];
	((UINT *) pn->selected_filetype->data.v)[0] = OWQ_U(owq);
	CACHE_WUNLOCK;
	if (previous > OWQ_U(owq)) {
		Cache_Clear();
	}
	return 0;
}

static ZERO_OR_ERROR FS_w_TS(struct one_wire_query *owq)
{
	ZERO_OR_ERROR ret = 0;
	if (OWQ_size(owq) == 0 || OWQ_offset(owq) > 0) {
		return 0;				/* do nothing */
	}

	CONTROLFLAGSLOCK;
	switch (OWQ_buffer(owq)[0]) {
	case 'C':
	case 'c':
		set_controlflags(&LocalControlFlags, TEMPSCALE_MASK, TEMPSCALE_BIT, temp_celsius);
		break;
	case 'F':
	case 'f':
		set_controlflags(&LocalControlFlags, TEMPSCALE_MASK, TEMPSCALE_BIT, temp_fahrenheit);
		break;
	case 'R':
	case 'r':
		set_controlflags(&LocalControlFlags, TEMPSCALE_MASK, TEMPSCALE_BIT, temp_rankine);
		break;
	case 'K':
	case 'k':
		set_controlflags(&LocalControlFlags, TEMPSCALE_MASK, TEMPSCALE_BIT, temp_kelvin);
		break;
	default:
		ret = -EINVAL;
	}
	CONTROLFLAGSUNLOCK;
	return ret;
}

static ZERO_OR_ERROR FS_w_PS(struct one_wire_query *owq)
{
	int ret = -EINVAL ;
	enum pressure_type pindex ;
	if (OWQ_size(owq) == 0 || OWQ_offset(owq) > 0) {
		return 0;				/* do nothing */
	}

	CONTROLFLAGSLOCK;
	for ( pindex = 0 ; pindex < pressure_end_mark ; ++pindex ) {
		if ( strcasecmp(  OWQ_buffer(owq), PressureScaleName(pindex) ) == 0 ) {
			set_controlflags(&LocalControlFlags, PRESSURESCALE_MASK, PRESSURESCALE_BIT, pindex);
			ret = 0 ;
			break;
		}
	}
	CONTROLFLAGSUNLOCK;
	return ret;
}

static ZERO_OR_ERROR FS_r_TS(struct one_wire_query *owq)
{
	return OWQ_format_output_offset_and_size_z(TemperatureScaleName(TemperatureScale(PN(owq))), owq);
}

static ZERO_OR_ERROR FS_r_PS(struct one_wire_query *owq)
{
	return OWQ_format_output_offset_and_size_z(PressureScaleName(PressureScale(PN(owq))), owq);
}
