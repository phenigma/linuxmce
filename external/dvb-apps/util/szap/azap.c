#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>

static char FRONTEND_DEV [80];
static char DEMUX_DEV [80];

#define CHANNEL_FILE "/.azap/channels.conf"

#define ERROR(x...)                                                     \
        do {                                                            \
                fprintf(stderr, "ERROR: ");                             \
                fprintf(stderr, x);                                     \
                fprintf (stderr, "\n");                                 \
        } while (0)

#define PERROR(x...)                                                    \
        do {                                                            \
                fprintf(stderr, "ERROR: ");                             \
                fprintf(stderr, x);                                     \
                fprintf (stderr, " (%s)\n", strerror(errno));		\
        } while (0)


typedef struct {
	char *name;
	int value;
} Param;

static const Param modulation_list [] = {
	{ "8VSB", VSB_8 },
	{ "16VSB", VSB_16 },
	{ "QAM_64", QAM_64 },
	{ "QAM_256", QAM_256 },
};

#define LIST_SIZE(x) sizeof(x)/sizeof(Param)


static
int parse_param (int fd, const Param * plist, int list_size, int *param)
{
	char c;
	int character = 0;
	int index = 0;

	while (1) {
		if (read(fd, &c, 1) < 1)
			return -1;	/*  EOF? */

		if ((c == ':' || c == '\n')
		    && plist->name[character] == '\0')
			break;

		while (toupper(c) != plist->name[character]) {
			index++;
			plist++;
			if (index >= list_size)	 /*  parse error, no valid */
				return -2;	 /*  parameter name found  */
		}

		character++;
	}

	*param = plist->value;

	return 0;
}


static
int parse_int(int fd, int *val)
{
	char number[11];	/* 2^32 needs 10 digits... */
	int character = 0;

	while (1) {
		if (read(fd, &number[character], 1) < 1)
			return -1;	/*  EOF? */

		if (number[character] == ':' || number[character] == '\n') {
			number[character] = '\0';
			break;
		}

		if (!isdigit(number[character]))
			return -2;	/*  parse error, not a digit... */

		character++;

		if (character > 10)	/*  overflow, number too big */
			return -3;	/*  to fit in 32 bit */
	};

	*val = strtol(number, NULL, 10);

	return 0;
}


static
int find_channel(int fd, const char *channel)
{
	int character = 0;

	while (1) {
		char c;

		if (read(fd, &c, 1) < 1)
			return -1;	/*  EOF! */

		if (c == ':' && channel[character] == '\0')
			break;

		if (toupper(c) == toupper(channel[character]))
			character++;
		else
			character = 0;
	};

	return 0;
}


static
int try_parse_int(int fd, int *val, const char *pname)
{
	int err;

	err = parse_int(fd, val);

	if (err)
		ERROR("error while parsing %s (%s)", pname,
		      err == -1 ? "end of file" :
		      err == -2 ? "not a number" : "number too big");

	return err;
}


static
int try_parse_param(int fd, const Param * plist, int list_size, int *param,
		    const char *pname)
{
	int err;

	err = parse_param(fd, plist, list_size, param);

	if (err)
		ERROR("error while parsing %s (%s)", pname,
		      err == -1 ? "end of file" : "syntax error");

	return err;
}


int parse(const char *fname, const char *channel,
	  struct dvb_frontend_parameters *frontend, int *vpid, int *apid)
{
	int fd;
	int err;
	int tmp;

	if ((fd = open(fname, O_RDONLY | O_NONBLOCK)) < 0) {
		PERROR ("could not open file '%s'", fname);
		perror ("");
		return -1;
	}

	if (find_channel(fd, channel) < 0) {
		ERROR("could not find channel '%s' in channel list", channel);
		return -2;
	}

	if ((err = try_parse_int(fd, &tmp, "frequency")))
		return -3;
	frontend->frequency = tmp;

	if ((err = try_parse_param(fd,
				   modulation_list, LIST_SIZE(modulation_list),
				   (int *) &frontend->u.vsb.modulation,
				   "modulation")))
		return -4;

	if ((err = try_parse_int(fd, vpid, "Video PID")))
		return -5;

	if ((err = try_parse_int(fd, apid, "Audio PID")))
		return -6;

	close(fd);

	return 0;
}


static
int set_pesfilter (int fd, int pid, dmx_pes_type_t type, int dvr)
{
        struct dmx_pes_filter_params pesfilter;

        if (pid <= 0 || pid >= 0x1fff)
                return 0;

        pesfilter.pid = pid;
        pesfilter.input = DMX_IN_FRONTEND;
        pesfilter.output = dvr ? DMX_OUT_TS_TAP : DMX_OUT_DECODER;
        pesfilter.pes_type = type;
        pesfilter.flags = DMX_IMMEDIATE_START;

        if (ioctl(fd, DMX_SET_PES_FILTER, &pesfilter) < 0) {
                PERROR ("ioctl(DMX_SET_PES_FILTER) for %s PID failed",
                        type == DMX_PES_AUDIO ? "Audio" :
                        type == DMX_PES_VIDEO ? "Video" : "??");
                return -1;
        }

        return 0;
}


static
int setup_frontend (int fe_fd, struct dvb_frontend_parameters *frontend)
{
	struct dvb_frontend_info fe_info;

	if (ioctl(fe_fd, FE_GET_INFO, &fe_info) < 0) {
		PERROR("ioctl FE_GET_INFO failed");
		return -1;
	}

	if (fe_info.type != FE_ATSC) {
		ERROR ("frontend device is not an ATSC (VSB/QAM) device");
		return -1;
	}

	printf ("tuning to %i Hz\n", frontend->frequency);

	if (ioctl(fe_fd, FE_SET_FRONTEND, frontend) < 0) {
		PERROR("ioctl FE_SET_FRONTEND failed");
		return -1;
	}

	return 0;
}


static
int check_frontend (int fe_fd)
{
	fe_status_t status;
	uint16_t snr, signal;
	uint32_t ber, uncorrected_blocks;

	do {
		ioctl(fe_fd, FE_READ_STATUS, &status);
		ioctl(fe_fd, FE_READ_SIGNAL_STRENGTH, &signal);
		ioctl(fe_fd, FE_READ_SNR, &snr);
		ioctl(fe_fd, FE_READ_BER, &ber);
		ioctl(fe_fd, FE_READ_UNCORRECTED_BLOCKS, &uncorrected_blocks);

		printf ("status %02x | signal %04x | snr %04x | "
			"ber %08x | unc %08x | ",
			status, signal, snr, ber, uncorrected_blocks);

		if (status & FE_HAS_LOCK)
			printf("FE_HAS_LOCK");

		usleep(1000000);

		printf("\n");
	} while (1);

	return 0;
}


static const char *usage = "\nusage: %s [-a adapter_num] [-f frontend_id] [-d demux_id] [-c conf_file] [-r] <channel name>\n\n";


int main(int argc, char **argv)
{
	struct dvb_frontend_parameters frontend_param;
	char *homedir = getenv ("HOME");
	char *confname = NULL;
	char *channel = NULL;
	int adapter = 0, frontend = 0, demux = 0, dvr = 0;
	int vpid, apid;
	int frontend_fd, audio_fd, video_fd;
	int opt;

	while ((opt = getopt(argc, argv, "hrn:a:f:d:c:")) != -1) {
		switch (opt) {
		case 'a':
			adapter = strtoul(optarg, NULL, 0);
			break;
		case 'f':
			frontend = strtoul(optarg, NULL, 0);
			break;
		case 'd':
			demux = strtoul(optarg, NULL, 0);
			break;
		case 'r':
			dvr = 1;
			break;
		case 'c':
			confname = optarg;
			break;
		case '?':
		case 'h':
		default:
			fprintf (stderr, usage, argv[0]);
			return -1;
		};
	}

	if (optind < argc)
		channel = argv[optind];

	if (!channel) {
		fprintf (stderr, usage, argv[0]);
		return -1;
	}

	snprintf (FRONTEND_DEV, sizeof(FRONTEND_DEV),
		  "/dev/dvb/adapter%i/frontend%i", adapter, frontend);

	snprintf (DEMUX_DEV, sizeof(DEMUX_DEV),
		  "/dev/dvb/adapter%i/demux%i", adapter, demux);

	printf ("using '%s' and '%s'\n", FRONTEND_DEV, DEMUX_DEV);

	if (!confname)
	{
		if (!homedir)
			ERROR ("$HOME not set");
		confname = malloc (strlen(homedir) + strlen(CHANNEL_FILE) + 1);
		memcpy (confname, homedir, strlen(homedir));
		memcpy (confname + strlen(homedir), CHANNEL_FILE,
	        	strlen(CHANNEL_FILE) + 1);
	}

	memset(&frontend_param, 0, sizeof(struct dvb_frontend_parameters));

	if (parse (confname, channel, &frontend_param, &vpid, &apid))
		return -1;

	if ((frontend_fd = open(FRONTEND_DEV, O_RDWR)) < 0) {
		PERROR ("failed opening '%s'", FRONTEND_DEV);
		return -1;
	}

	if (setup_frontend (frontend_fd, &frontend_param) < 0)
		return -1;

        if ((video_fd = open(DEMUX_DEV, O_RDWR)) < 0) {
                PERROR("failed opening '%s'", DEMUX_DEV);
                return -1;
        }

	printf ("video pid 0x%04x, audio pid 0x%04x\n", vpid, apid);
	if (set_pesfilter (video_fd, vpid, DMX_PES_VIDEO, dvr) < 0)
		return -1;

	if ((audio_fd = open(DEMUX_DEV, O_RDWR)) < 0) {
                PERROR("failed opening '%s'", DEMUX_DEV);
                return -1;
        }

	if (set_pesfilter (audio_fd, apid, DMX_PES_AUDIO, dvr) < 0)
		return -1;

	check_frontend (frontend_fd);

	close (audio_fd);
	close (video_fd);
	close (frontend_fd);

	return 0;
}

