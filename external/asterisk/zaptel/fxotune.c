/* 
 * This file and contents thereof are licensed under the terms and
 * conditions of the GNU Public License version 2.  For more information
 * (including terms and conditions) see http://www.gnu.org/
 *
 * fxotune.c -- A utility for tuning the various settings on the fxo
 * 		modules for the TDM400 cards.
 *
 * by Matthew Fredrickson
 * 
 * (C) 2004-2005 Digium, Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#ifdef STANDALONE_ZAPATA
#include "zaptel.h"
#else
#include <linux/zaptel.h>
#endif
#include "wctdm.h"
#include "fxotune.h"

#define TEST_DURATION 4000			/* 4000 samples (or 500 ms) of test */
#define BUFFER_LENGTH 8000			/* 4000 sample buffers */
#define SKIP_BYTES 1600

#define PI 3.14

static char *zappath = "/dev/zap";
static char *configfile = "/etc/fxotune.conf";

static char *usage =
"Usage: fxotune\n"
"	-i <number> : Tests for FXO modules and sets echo coefficients, dialing <number> to clear dialtone\n"
"	-s          : Sets the FXO modules echo coefficients on your system to the best settings\n";

#define OUT_OF_BOUNDS(x) ((x) < 0 || (x) > 255)

static short outbuf[TEST_DURATION];
static int debug = 0;


/* Generates a tone of hz frequency.  Index is the current sample
 * to begenerated.  For a normal waveform you need to increment
 * this every time you execute the function.
 * Returns a 16bit slinear sample. */
static short inline gentone(int hz, int index)
{
	return sinf(index * 2 * PI * hz/8000);
}

/* Returns the power of the buffer of samples in 16bit slinear format.
 * power function = (sum of squares) - (square of sums).
 *
 * TODO: make the function stateless so that you can have some sort of
 * progressive power calculation on the line */
static float power_of(void *prebuf, int bufsize, int short_format)
{
	float sum_of_squares = 0, square_of_sums = 0;
	int numsamples = 0;
	float finalanswer = 0;
	short *sbuf = (short*)prebuf;
	float *fbuf = (float*)prebuf;
	int i = 0;

	if (short_format) {
		/* idiot proof checks */
		if (bufsize <= 0)
			return -1;

		if (bufsize % 2 != 0)
			return -2;

		numsamples = bufsize / 2;

		for (i = 0; i < numsamples; i++) {
#if 0
			sum_of_squares += ((float)sbuf[i] * (float)sbuf[i])/(float)32768;
#endif
			sum_of_squares += ((float)sbuf[i] * (float)sbuf[i]);
			if (sbuf[i] > 0) {
				square_of_sums += (float)sbuf[i];
			} else {
				sbuf[i] *= -1;
				square_of_sums += (float)sbuf[i];
			}
		}
	} else {
		/* Version for float inputs */
		for (i = 0; i < bufsize; i++) {
			sum_of_squares += (fbuf[i] * fbuf[i]);
			square_of_sums += fbuf[i];
		}
	}

	if (debug) printf("sum_of_squares = %f\n", sum_of_squares);

	square_of_sums *= square_of_sums; /* sums ^ 2 */

	if (debug) printf("square_of_sums = %f\n", square_of_sums);

	finalanswer = square_of_sums - sum_of_squares; 

	if (debug) printf("finalanswer = %f\n", finalanswer);

	if (finalanswer < 0) {
		printf("Error: Final answer negative number %f\n", finalanswer);
		return -3;
	}
#if 0
		finalanswer = finalanswer * (float)-1;
#endif

	return sqrtf(finalanswer);
}

#if 0
int fill_outputdata(int freq)
{
	int i = 0;

	for (i = 0; i < TESTDURATIION; i++)
		outbuf[i] = gentone(freq, i);

		

	int randdev;
	int cursize = 0;
	int needlen = TEST_DURATION * 2;
	int res;

	fprintf(stdout, "Getting random impulse data\n");
	randdev = open("/dev/urandom", O_RDONLY);
	if (randdev < 0) {
		fprintf(stdout, "Unable to open /dev/random: %s\n", strerror(errno));
		return -1;
	}
	
	while (needlen) {
		res = read(randdev, &outbuf[cursize], needlen);
		if (res <= 0) {
			fprintf(stdout, "WARNING: could not read from /dev/random: %s\n", strerror(errno));
			return -1;
		}
		cursize += res;
		needlen -= res;
	}
	
	fprintf(stdout, "Succesfully filled the random data buffer\n");
	close(randdev);

	return 0;
}
#endif


#if 0
/* Returns index in echocan table with the lowest power pulse readback
 * -1 means the device is not an FXO module or fails */
static int echo_tune(int whichzap, const char *dialstr)
{
	int bestval = -1;
	int bestindex = -1;
	int i = 0, j=0;
	int x;
	int res = 0;
	int total = sizeof(echo_trys) / sizeof(struct wctdm_echo_coefs);
	short inbuf[BUFFER_LENGTH];
	struct zt_bufferinfo bi;
	struct zt_dialoperation dop;
	struct wctdm_echo_coefs coefs;

	/* Set echo settings */
	memset(&coefs, 0, sizeof(coefs));
	if (ioctl(whichzap, WCTDM_SET_ECHOTUNE, &coefs)) {
		fprintf(stdout, "Skipping non-TDM / non-FXO\n");
		return -1;
	}

	x = 1;
	if (ioctl(whichzap, ZT_SETLINEAR, &x)) {
		fprintf(stderr, "Unable to set channel to signed linear mode.\n");
		return -1;
	}

	memset(&bi, 0, sizeof(bi));
	if (ioctl(whichzap, ZT_GET_BUFINFO, &bi)) {
		fprintf(stderr, "Unable to get buffer information!\n");
		return -1;
	}
	bi.numbufs = 2;
	bi.bufsize = BUFFER_LENGTH;
	bi.txbufpolicy = ZT_POLICY_IMMEDIATE;
	bi.rxbufpolicy = ZT_POLICY_IMMEDIATE;
	if (ioctl(whichzap, ZT_SET_BUFINFO, &bi)) {
		fprintf(stderr, "Unable to set buffer information!\n");
		return -1;
	}

	/* XXX May need to put in a loop XXX */
	for (j=0;j<(total + 15)/16;j++) {
		printf("Going off hook!\n");
		/* Take off hook */
		x = ZT_OFFHOOK;
		if(ioctl(whichzap, ZT_HOOK, &x)) {
			fprintf(stdout, "Unable to set hook state.\n");
			return -1;
		}
		sleep(2);
		printf("Off hook (event = %d)!\n", x);
		memset(&dop, 0, sizeof(dop));
		dop.op = ZT_DIAL_OP_REPLACE;
		dop.dialstr[0] = 'T';
		strncpy(dop.dialstr + 1, dialstr, sizeof(dop.dialstr) - 1);
		printf("Dialing...\n");
		if (ioctl(whichzap, ZT_DIAL, &dop)) {
			fprintf(stderr, "Unable to dial!\n");
			return -1;
		}
		sleep(2);
		printf("Finished Dialing...\n");

		for (i = j * 16; (i < total) && (i < (j+1) * 16); i++) {
			/* Set echo settings */
			if (ioctl(whichzap, WCTDM_SET_ECHOTUNE, &echo_trys[i])) {
				fprintf(stderr, "Unable to set echo params: %s\n", strerror(errno));
				return -1;
			}
	
			x = ZT_FLUSH_READ | ZT_FLUSH_WRITE | ZT_FLUSH_EVENT;
			if (ioctl(whichzap, ZT_FLUSH, &x)) {
				fprintf(stderr, "Unable to flush I/O: %s\n", strerror(errno));
				return -1;
			}
	
			/* write samples */
			res = write(whichzap, outbuf, TEST_DURATION * 2);
			if (res < 0) {
				fprintf(stdout, "Unable to write: %s\n", strerror(errno));
				return -1;
			}

			if (res != TEST_DURATION * 2) {
				fprintf(stdout, "Only could write %d of %d bytes.\n", res, TEST_DURATION * 2);
				return -1;
			}

			res = read(whichzap, inbuf, BUFFER_LENGTH * 2);
			if (res < 0) {
				fprintf(stdout, "Error in read: %s\n", strerror(errno));
				return -1;
			}

			if (res != BUFFER_LENGTH * 2) {
				fprintf(stdout, "Only could read %d of %d bytes.\n", res, BUFFER_LENGTH * 2);
				return -1;
			}
		
			res = process_readbuf(inbuf);
			printf("Test %d: %d\n", i + 1, res);
			/* Check to see if the echo values */
			if ((res < bestval) || (bestval < 0)) {
				bestval = res;
				bestindex = i;
			}
			usleep(100000);
		}
		printf("Going on hook!\n");
		/* Take off hook */
		x = ZT_ONHOOK;
		if(ioctl(whichzap, ZT_HOOK, &x)) {
			fprintf(stdout, "Unable to set hook state.\n");
			return -1;
		}
		sleep(2);
		printf("On hook (event = %d)!\n", x);
	}
	printf("Best is %d from index %d\n", bestval, bestindex);

	return 0;

}
#endif

/* Tune the line impedance.  Look for best response range */
static int acim_tune(int whichzap, char *dialstr)
{
	int i = 0, freq = 0, acim = 0;
	int res = 0, x = 0;
	struct zt_bufferinfo bi;
	struct zt_dialoperation dop;
	struct wctdm_echo_coefs coefs;
	short inbuf[BUFFER_LENGTH]; 
	int lowest = 0;
	FILE *outfile = NULL;
	float acim_results[16];


	outfile = fopen("/tmp/fxotune.vals", "w");
	if (!outfile) {
		fprintf(stdout, "Cannot create /tmp/txotune.vals\n");
		return -1;
	}

	/* Set echo settings */
	memset(&coefs, 0, sizeof(coefs));
	if (ioctl(whichzap, WCTDM_SET_ECHOTUNE, &coefs)) {
		fprintf(stdout, "Skipping non-TDM / non-FXO\n");
		return -1;
	}

	x = 1;
	if (ioctl(whichzap, ZT_SETLINEAR, &x)) {
		fprintf(stderr, "Unable to set channel to signed linear mode.\n");
		return -1;
	}

	memset(&bi, 0, sizeof(bi));
	if (ioctl(whichzap, ZT_GET_BUFINFO, &bi)) {
		fprintf(stderr, "Unable to get buffer information!\n");
		return -1;
	}
	bi.numbufs = 2;
	bi.bufsize = BUFFER_LENGTH;
	bi.txbufpolicy = ZT_POLICY_IMMEDIATE;
	bi.rxbufpolicy = ZT_POLICY_IMMEDIATE;
	if (ioctl(whichzap, ZT_SET_BUFINFO, &bi)) {
		fprintf(stderr, "Unable to set buffer information!\n");
		return -1;
	}
	x = ZT_OFFHOOK;
	if (ioctl(whichzap, ZT_HOOK, &x)) {
		fprintf(stderr, "Cannot bring fd %d off hook", whichzap);
		return -1;
	}

	for (acim = 0; acim < 16; acim++) {
		float freq_results[15];
		int needtoreset = 0;

		coefs.acim = acim;
		if (ioctl(whichzap, WCTDM_SET_ECHOTUNE, &coefs)) {
			fprintf(stderr, "Unable to set impedance on fd %d\n", whichzap);
			return -1;
		}

		for (freq = 200; freq <=3000; freq+=200, needtoreset++) {
			/* Fill the output buffers */
			for (i = 0; i < TEST_DURATION; i++)
				outbuf[i] = gentone(freq, i);

			/* Prepare line for data */
			if (needtoreset > 8) {
				/* Do line hookstate reset */
				x = ZT_ONHOOK;

				if (ioctl(whichzap, ZT_HOOK, &x)) {
					fprintf(stderr, "Unable to hang up fd %d\n", whichzap);
					return -1;
				}

				sleep(2);
				x = ZT_OFFHOOK;
				if (ioctl(whichzap, ZT_HOOK, &x)) {
					fprintf(stderr, "Cannot bring fd %d off hook", whichzap);
					return -1;
				}

				memset(&dop, 0, sizeof(dop));
				dop.op = ZT_DIAL_OP_REPLACE;
				dop.dialstr[0] = 'T';
				strncpy(dop.dialstr + 1, dialstr, sizeof(dop.dialstr) - 1);
				printf("Dialing...\n");
				if (ioctl(whichzap, ZT_DIAL, &dop)) {
					fprintf(stderr, "Unable to dial!\n");
					return -1;
				}
				sleep(2);

				needtoreset = 0;
			}

			/* Flush buffers */
			x = ZT_FLUSH_READ | ZT_FLUSH_WRITE | ZT_FLUSH_EVENT;
			if (ioctl(whichzap, ZT_FLUSH, &x)) {
				fprintf(stderr, "Unable to flush I/O: %s\n", strerror(errno));
				return -1;
			}
	
			/* send data out on line */
			res = write(whichzap, outbuf, BUFFER_LENGTH);
			if (res != BUFFER_LENGTH) {
				fprintf(stderr, "Could not write all data to line\n");
				return -1;
			}

			/* read return response */
			res = read(whichzap, inbuf, BUFFER_LENGTH);
			if (res != BUFFER_LENGTH) {
				fprintf(stderr, "Could not fill input buffer\n");
				return -1;
			}

			/* calculate power of response */
			
			freq_results[(freq/200)-1] = power_of(inbuf+SKIP_BYTES, BUFFER_LENGTH-SKIP_BYTES, 1);
			if (debug) fprintf(outfile, "%d,%d,%f\n", acim, freq, freq_results[(freq/200)-1]);
		}
		acim_results[acim] = power_of(freq_results, 15, 0);
	}

	for (i = 0; i < 16; i++)
		fprintf(outfile, "acim_results[%d] = %f\n", i, acim_results[i]);
	/* Find out what the "best" impedance is for the line */
	lowest = 0;
	for (i = 0; i < 16; i++) {
		if (acim_results[i] < acim_results[lowest]) {
			lowest = i;
		}
	}

	return lowest;
}
	
int main (int argc , char **argv)
{
	char zapdev[80] = "";
	int i = 0;
	int fd;
	int res = 0;
	int configfd;
	FILE *fp = NULL;

	if ((argc < 2) || (argc > 3)) {
		/* Show usage */
		fputs(usage, stdout);
		return -1;
	}

	if (!strcasecmp(argv[1], "-s")) {
set:

		fp = fopen(configfile, "r");

		for (i = 0;res != EOF; i++) {
			struct wctdm_echo_coefs mycoefs;
			char completezappath[56] = "";
			int myzap,myacim,mycoef1,mycoef2,mycoef3,mycoef4,mycoef5,mycoef6,mycoef7,mycoef8;
	
	
			res = fscanf(fp, "%d=%d,%d,%d,%d,%d,%d,%d,%d,%d",&myzap,&myacim,&mycoef1,
					&mycoef2,&mycoef3,&mycoef4,&mycoef5,&mycoef6,&mycoef7,
					&mycoef8);
	
			if (res == EOF) {
				break;
			}
	
			/* Check to be sure conversion is done correctly */
			if (OUT_OF_BOUNDS(myacim) || OUT_OF_BOUNDS(mycoef1)||
				OUT_OF_BOUNDS(mycoef2)|| OUT_OF_BOUNDS(mycoef3)||
				OUT_OF_BOUNDS(mycoef4)|| OUT_OF_BOUNDS(mycoef5)||
				OUT_OF_BOUNDS(mycoef6)|| OUT_OF_BOUNDS(mycoef7)|| OUT_OF_BOUNDS(mycoef8)) {
	
				fprintf(stdout, "Bounds check error on inputs from %s:%d\n", configfile, i+1);
				return -1;
			}
	
			mycoefs.acim = myacim;
			mycoefs.coef1 = mycoef1;
			mycoefs.coef2 = mycoef2;
			mycoefs.coef3 = mycoef3;
			mycoefs.coef4 = mycoef4;
			mycoefs.coef5 = mycoef5;
			mycoefs.coef6 = mycoef6;
			mycoefs.coef7 = mycoef7;
			mycoefs.coef8 = mycoef8;
		
			snprintf(completezappath, sizeof(completezappath), "%s/%d", zappath, myzap);
			fd = open(completezappath, O_RDWR);

			if (fd < 0) {
				fprintf(stdout, "open error on %s: %s\n", completezappath, strerror(errno));
				return -1;
			}

			if (ioctl(fd, WCTDM_SET_ECHOTUNE, &mycoefs)) {
				fprintf(stdout, "%s: %s\n", completezappath, strerror(errno));
				return -1;
			}

			close(fd);
		}

		fclose(fp);
	
		fprintf(stdout, "fxotune: successfully set echo coeffecients on FXO modules\n");
		return 0;
	}
	
	if (!strcasecmp(argv[1], "-i")) {
		if (argc != 3) {
			/* Show usage */
			fputs(usage, stdout);
			return -1;
		}
		configfd = open(configfile, O_CREAT|O_TRUNC|O_WRONLY);

		if (configfd < 0) {
			fprintf(stderr, "open: %s\n", strerror(errno));
			return -1;
		}

		for (i = 0; i < 252; i++) {
			snprintf(zapdev, sizeof(zapdev), "%s/%d", zappath, i+1);

			fd = open(zapdev, O_RDWR);
			if (fd < 0) {
				if (debug)
					fprintf(stdout, "%s absent: %s\n", zapdev, strerror(errno));
				continue;
			}

			res = acim_tune(fd, argv[2]); /* Shouldn't matter what digit we press */

			close(fd);
			if (res > -1) {
			/* Do output to file */
				int len = 0;
				static char output[255] = "";
#if 0
				snprintf(output, sizeof(output), "%d=%d,%d,%d,%d,%d,%d,%d,%d,%d\n", i+1,
						echo_trys[i].acim, echo_trys[i].coef1, echo_trys[i].coef2,
						echo_trys[i].coef3, echo_trys[i].coef4, echo_trys[i].coef5,
						echo_trys[i].coef6, echo_trys[i].coef7, echo_trys[i].coef8 );
#endif

				snprintf(output, sizeof(output), "%d=%d,%d,%d,%d,%d,%d,%d,%d,%d\n", i+1, res,
						0, 0, 0, 0, 0, 0, 0, 0);
				len = strlen(output);
				res = write(configfd, output, strlen(output));
				if (res != len) {
					fprintf(stdout, "Unable to write line \"%s\" to file.\n", output);
					return -1;
				}
			}
		}

		close(configfd);
		/* Some getto goto hackery to make this more convenient */
		goto set;
	}

	fputs(usage, stdout);

	return 0;
}
