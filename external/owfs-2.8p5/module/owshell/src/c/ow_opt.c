/*
$Id: ow_opt.c,v 1.18 2010/04/24 15:38:08 alfille Exp $
    OWFS -- One-Wire filesystem
    OWHTTPD -- One-Wire Web Server
    Written 2003 Paul H Alfille
    email: palfille@earthlink.net
    Released under the GPL
    See the header file: ow.h for full attribution
    1wire/iButton system from Dallas Semiconductor
*/

/* ow_opt -- owlib specific command line options processing */

#include "owshell.h"

// globals
int hexflag = 0 ;
int slashflag = 0 ;
int size_of_data = -1 ;
int offset_into_data = 0 ;

static void OW_parsevalue(int *var, const ASCII * str);

const struct option owopts_long[] = {
	{"help", no_argument, NULL, 'h'},
	{"server", required_argument, NULL, 's'},
	{"Celsius", no_argument, NULL, 'C'},
	{"Fahrenheit", no_argument, NULL, 'F'},
	{"Kelvin", no_argument, NULL, 'K'},
	{"Rankine", no_argument, NULL, 'R'},
	{"version", no_argument, NULL, 'V'},
	{"format", required_argument, NULL, 'f'},

	{"hex", no_argument, &hexflag, 1 },
	{"HEX", no_argument, &hexflag, 1 },
	{"hexidecimal", no_argument, &hexflag, 1 },
	{"HEXIDECIMAL", no_argument, &hexflag, 1 },

	{"dir", no_argument, &slashflag, 1 },
	{"slash", no_argument, &slashflag, 1 },
	{"SLASH", no_argument, &slashflag, 1 },

	{"size", required_argument, NULL, 300 },
	{"SIZE", required_argument, NULL, 300 },
	{"BYTES", required_argument, NULL, 300 },
	{"bytes", required_argument, NULL, 300 },

	{"offset", required_argument, NULL, 301 },
	{"OFFSET", required_argument, NULL, 301 },
	{"start", required_argument, NULL, 301 },
	{"START", required_argument, NULL, 301 },

	{"timeout_network", required_argument, NULL, 307,},	// timeout -- tcp wait
	{"timeout_server", required_argument, NULL, 308,},	// timeout -- server wait
	{"autoserver", no_argument, NULL, 275},

	{0, 0, 0, 0},
};

/* Parses one argument */
void owopt(const int c, const char *arg)
{
	//printf("Option %c (%d) Argument=%s\n",c,c,SAFESTRING(arg)) ;
	switch (c) {
	case 'h':
		ow_help();
		Exit(0);
	case 'C':
		set_semiglobal(&SemiGlobal, TEMPSCALE_MASK, TEMPSCALE_BIT, temp_celsius);
		break;
	case 'F':
		set_semiglobal(&SemiGlobal, TEMPSCALE_MASK, TEMPSCALE_BIT, temp_fahrenheit);
		break;
	case 'R':
		set_semiglobal(&SemiGlobal, TEMPSCALE_MASK, TEMPSCALE_BIT, temp_rankine);
		break;
	case 'K':
		set_semiglobal(&SemiGlobal, TEMPSCALE_MASK, TEMPSCALE_BIT, temp_kelvin);
		break;
	case 'V':
		printf("owshell version:\n\t" VERSION "\n");
		Exit(0);
	case 's':
		ARG_Net(arg);
		break;
	case 'f':
		if (!strcasecmp(arg, "f.i"))
			set_semiglobal(&SemiGlobal, DEVFORMAT_MASK, DEVFORMAT_BIT, fdi);
		else if (!strcasecmp(arg, "fi"))
			set_semiglobal(&SemiGlobal, DEVFORMAT_MASK, DEVFORMAT_BIT, fi);
		else if (!strcasecmp(arg, "f.i.c"))
			set_semiglobal(&SemiGlobal, DEVFORMAT_MASK, DEVFORMAT_BIT, fdidc);
		else if (!strcasecmp(arg, "f.ic"))
			set_semiglobal(&SemiGlobal, DEVFORMAT_MASK, DEVFORMAT_BIT, fdic);
		else if (!strcasecmp(arg, "fi.c"))
			set_semiglobal(&SemiGlobal, DEVFORMAT_MASK, DEVFORMAT_BIT, fidc);
		else if (!strcasecmp(arg, "fic"))
			set_semiglobal(&SemiGlobal, DEVFORMAT_MASK, DEVFORMAT_BIT, fic);
		else {
			fprintf(stderr, "Unrecognized format type %s\n", arg);
			Exit(1);
		}
		break;
	case 275:					// autoserver
#if OW_ZERO
		if (libdnssd == NULL) {
			fprintf(stderr, "Zeroconf/Bonjour is disabled since dnssd library isn't found.\n");
			Exit(0);
		} else {
			OW_Browse();
		}
#else
		fprintf(stderr, "Zeroconf/Bonjour is disabled since it's compiled without support.\n");
		Exit(0);
#endif
		break;
	case 300:
		OW_parsevalue(&size_of_data, arg);
		if ( size_of_data < 0 || size_of_data > 64000 ) {
			fprintf(stderr, "Bad data size value. (%d).\n", size_of_data) ;
			Exit(1);
		}
		break ;
	case 301:
		OW_parsevalue(&offset_into_data, arg);
		if ( offset_into_data < 0 || offset_into_data > 64000 ) {
			fprintf(stderr, "Bad data offset value. (%d).\n", offset_into_data) ;
			Exit(1);
		}
		break ;
	case 307:
	case 308:
		OW_parsevalue(&((int *) &Globals.timeout_volatile)[c - 301], arg);
	case 0:
		break;
	default:
		Exit(1);
	}
}

void ARG_Net(const char *arg)
{
	++count_inbound_connections;
	if (count_inbound_connections > 1) {
		fprintf(stderr, "Cannot link to more than one owserver. (%s and %s).\n", owserver_connection->name, arg);
		Exit(1);
	}
	owserver_connection->name = strdup(arg);
}

static void OW_parsevalue(int *var, const ASCII * str)
{
	int I;
	errno = 0;
	I = strtol(str, NULL, 10);
	if (errno) {
		fprintf(stderr, "Bad configuration value %s\n", str);
		Exit(1);
	}
	var[0] = I;
}
