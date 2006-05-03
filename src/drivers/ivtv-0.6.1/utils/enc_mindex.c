//++MTY use e.g., mpegcat -dV /video/1011_20030808233000_20030809000000.nuv

/*
#
# MPEG2Parser class testing tool
#
# $Id: mindex.c,v 1.8 2003/12/16 18:50:56 root Exp root $
#
# Copyright (C) 2001-2003 Kees Cook
# kees@outflux.net, http://outflux.net/
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# http://www.gnu.org/copyleft/gpl.html
#
*/

/*
 * buffer management inspired by mplayer
 */
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <string.h>
#include <errno.h>
#include "mpeg2structs.h"

#define VERBOSE 0
#define STATUS 1

int mindex(char *, char *, char *);

// general stuff
int debug = 0;
int dvd_names = 0;

// file position options
loff_t begin_at = 0;
loff_t num_bytes = 0;

typedef enum {
	PACK_NONE,
	PACK_SPECIAL,
	PACK_PES_SIMPLE,	// packet length == data length
	PACK_PES_COMPLEX,	// crazy headers need skipping
} packet_type;

typedef enum {
	SUBID_NONE,		// no sub id (sid=0)
	SUBID_DATA,		// first PES data byte
} subid_type;

// FIXME: 
//   - add "valid scope" flags (eg some markers only valid in Video stream)
//   - add "struct size" field for auto-forwarding past each marker
typedef struct {
	// the byte value match for the packet tags
	uint8_t code_match_lo;	// low end of the range of matches
	uint8_t code_match_hi;	// high end of the range of matches

	// what kind of PES is it?
	packet_type packet;

	// how do we find the stream sub id
	subid_type subid_method;

	// misc stuff... sorting types?
} packet_tag_info;

packet_tag_info packet_tags[] = {
	{0x00, 0x00, PACK_SPECIAL, SUBID_NONE},	// pic start
	{0x01, 0xAF, PACK_SPECIAL, SUBID_NONE},	// video slices
	{0xB0, 0xB1, PACK_SPECIAL, SUBID_NONE},	// reserved
	{0xB2, 0xB5, PACK_SPECIAL, SUBID_NONE},	// user data, sequences
	{0xB6, 0xB6, PACK_SPECIAL, SUBID_NONE},	// reserved
	{0xB7, 0xB9, PACK_SPECIAL, SUBID_NONE},	// sequence, gop, end
	{0xBA, 0xBA, PACK_SPECIAL, SUBID_NONE},	// pack
	{0xBB, 0xBB, PACK_PES_SIMPLE, SUBID_NONE},	// system: same len as PES
	{0xBC, 0xBC, PACK_PES_SIMPLE, SUBID_NONE},	// PES: prog stream map
	{0xBD, 0xBD, PACK_PES_COMPLEX, SUBID_DATA},	// PES: priv 1
	{0xBE, 0xBF, PACK_PES_SIMPLE, SUBID_NONE},	// PES: padding, priv 2
	{0xC0, 0xDF, PACK_PES_COMPLEX, SUBID_NONE},	// PES: Audio
	{0xE0, 0xEF, PACK_PES_COMPLEX, SUBID_NONE},	// PES: Video
	{0xF0, 0xF2, PACK_PES_SIMPLE, SUBID_NONE},	// PES: ecm, emm, dsmcc
	{0xF3, 0xF7, PACK_PES_COMPLEX, SUBID_NONE},	// PES: iso 13522/h2221a-d
	{0xF8, 0xF8, PACK_PES_SIMPLE, SUBID_NONE},	// PES: h2221e
	{0xF9, 0xF9, PACK_PES_COMPLEX, SUBID_NONE},	// PES: ancillary
	{0xFA, 0xFE, PACK_PES_SIMPLE, SUBID_NONE},	// PES: reserved
	{0xFF, 0xFF, PACK_PES_SIMPLE, SUBID_NONE},	// PES: prog stream dir
	{0, 0, PACK_NONE, SUBID_NONE}	// end of list
};

#define BUFFER_SIZE	(1024*512)
FILE *fp = NULL;
uint8_t buffer[BUFFER_SIZE];
loff_t offset = 0;
size_t buffer_min = 0;
size_t buffer_max = 0;

loff_t buffer_tell()
{
	return offset - buffer_max + buffer_min;
}

// can point to a file offset to refill from
void buffer_refill()
{
	size_t got;

	//fprintf(stderr,"filling...\n");

	if (buffer_min < buffer_max) {
		memmove(buffer, buffer + buffer_min, buffer_max - buffer_min);
	}
	buffer_max -= buffer_min;
	buffer_min = 0;

	got =
	    fread(buffer + buffer_max, sizeof(uint8_t),
		  BUFFER_SIZE - buffer_max, fp);
	if (got < 0) {
		perror("fread");
		exit(2);
	}
	offset += got;
	buffer_max += got;

	//fprintf(stderr,"bump %d more\n",got);
	usleep(1);
}

void buffer_seek(loff_t location)
{
	// is this location within our current buffer?
	if (location < offset && location >= buffer_tell()) {
		buffer_min += location - buffer_tell();
	} else {
		// outside: clear the buffer 
		buffer_min = buffer_max = 0;	// clear buffer
		offset = location;
		if (fseeko(fp, offset, SEEK_SET) < 0) {
			fprintf(stderr, "fseeko failed: err = %s\n",
				strerror(errno));
			exit(1);
		}
	}
	//don't do this... way too slow
	//buffer_refill();
}

int buffer_start(char *filename)
{
	if (fp)
		fclose(fp);
	fp = NULL;

	if (!(fp = fopen(filename, "r"))) {
		perror(filename);
		return -1;
	}

	offset = 0;
	buffer_min = 0;
	buffer_max = 0;
	return 0;
}

int buffer_get_byte(uint8_t * byte)
{
	if (!byte)
		return -2;
	if (buffer_min == buffer_max)
		buffer_refill();
	if (buffer_min == buffer_max)
		return -1;

	*byte = buffer[buffer_min++];
	return 0;
}

int buffer_look_ahead(uint8_t * bytes, int count)
{
	int i;

	if (!bytes)
		return -2;
	if (count <= 0)
		return 0;
	if (count >= BUFFER_SIZE)
		return -3;

	if (buffer_min + count > buffer_max)
		buffer_refill();
	if (buffer_min + count > buffer_max)
		return -1;

	for (i = 0; i < count; i++) {
		*(bytes++) = buffer[buffer_min + i];
		//fprintf(stderr,"\tsaw: 0x%02X\n",buffer[buffer_min+i]);
	}

	return 0;
}

uint8_t packet_buffer[65536];	// max size of a double byte
uint8_t *packet_start;
uint16_t packet_size;

loff_t file_offset_of_last_packet = 0;
uint32_t framecount = 0;

typedef union {
	struct {
		uint32_t padding:5, broken:1, closed:1, frame:6, second:6,
		    padding2:1, minute:6, hour:5, drop:1;
	};
	struct {
		uint32_t data;
	};
} gop_header_t;

struct mpeg_index_entry {
	unsigned int frame;
	gop_header_t timestamp;
	unsigned long long offset;
};

FILE *indexfd = NULL;
struct mpeg_index_entry last_index_written;
int last_was_gop_packet = 0;

static inline uint32_t bswap(uint32_t x)
{
      asm("bswap %0": "=r"(x):"0"(x));
	return x;
}

char *timestamp_to_string(char *str, gop_header_t timestamp)
{
	static char buf[256];
	char *p = (str) ? str : buf;
	snprintf(p, sizeof(buf), "%d:%02d:%02d:%d", timestamp.hour,
		 timestamp.minute, timestamp.second, timestamp.frame);
	return p;
}

static inline gop_header_t make_timestamp(int hour, int minute, int second,
					  int frame)
{
	gop_header_t gop;
	gop.hour = hour;
	gop.minute = minute;
	gop.second = second;
	gop.frame = frame;
	return gop;
}

loff_t process_packet(uint8_t code, char *bstatus)
{
	uint8_t bytes[32];
	int i;

	unsigned int length, header_len;
	loff_t position = 0;

	//unsigned long long system_clock_ref_base;   // "H"igh, "M"edium, "L"ow bits
	//unsigned long system_clock_ref_ext; // "E"
	//unsigned int program_mux_rate;      // "R"
	//unsigned int pack_stuffing_length;  // "S"

	loff_t file_offset = buffer_tell() - 4;

	// initialize "unknown" packet length
	header_len = length = 0;

	last_was_gop_packet = 0;
	// find the packet type
	for (i = 0; packet_tags[i].packet != PACK_NONE; i++) {
		if (code >= packet_tags[i].code_match_lo
		    && code <= packet_tags[i].code_match_hi) {
			int sid = 0;

			switch (packet_tags[i].packet) {
			case PACK_SPECIAL:
				// actually, nothing special
				break;
			case PACK_PES_SIMPLE:
			case PACK_PES_COMPLEX:
				if (packet_tags[i].packet == PACK_PES_COMPLEX) {
					if (buffer_look_ahead(bytes, 5) != 0)
						return 0;
					header_len = 5 + bytes[4];

					if (dvd_names
					    && packet_tags[i].subid_method ==
					    SUBID_DATA
					    && buffer_look_ahead(bytes,
								 header_len +
								 1) == 0) {
						sid = bytes[header_len];
					}
				} else {
					if (buffer_look_ahead(bytes, 2) != 0)
						return 0;
				}
				length = bytes[1] | (bytes[0] << 8);

				// get position updated for next packet location
				if ((code & PES_TYPE_MASK_video) ==
				    PES_TYPE_video)
					break;	// skip position update
				position = buffer_tell() + length + 2;
				break;
			default:
			case PACK_NONE:
				// FIXME: not possible!
				break;
			}

			break;	// found a match
		}
	}

	if (code == PES_TYPE_pack_start) {
		//++MTY
		file_offset_of_last_packet = file_offset;

		if (buffer_look_ahead(bytes, 10) == 0) {

			//|   0   |   1   |   2   |   3   |   4   |   5   |   6   |
			// 76543210765432107654321076543210765432107654321076543210
			// xxHHHxMMMMMMMMMMMMMMMxLLLLLLLLLLLLLLLxEEEEEEEEExRRRRRRRR
			//   333 222222222211111 111110000000000 000000000 22111111// bit
			//   210 987654321098765 432109876543210 876543210 10987654//     number
			//     -  -       -    -  -       -    -  -      -        -
			//     2  2       2    1  1       0    -  0      -        1// left
			//     7  8       0    2  3       5    3  7      1        4//      shift
			//
			//|   7   |   8   |   9   |
			// 765432107654321076543210
			// RRRRRRRRRRRRRRxxrrrrrSSS
			// 11110000000000  00000000   // bit
			// 32109876543210  43210210   //     number
			//        -     -      -  -
			//        0     -      -  0   // left
			//        6     2      3  0   //      shift

			/*int system_clock_ref_base = (((bytes[0] & 0x38) << 27) |
			   ((bytes[0] & 0x03) << 28) |
			   ((bytes[1] & 0xFF) << 20) |
			   ((bytes[2] & 0xF8) << 12) |
			   ((bytes[2] & 0x03) << 13) |
			   ((bytes[3] & 0xFF) << 5) |
			   ((bytes[4] & 0xF8) >> 3));
			   int system_clock_ref_ext = (((bytes[4] & 0x03) << 7) |
			   ((bytes[5] & 0xFE) >> 1));
			   int program_mux_rate = (((bytes[6] & 0xFF) << 14) |
			   ((bytes[7] & 0xFF) << 6) |
			   ((bytes[8] & 0xFC) >> 2));
			   int pack_stuffing_length = (bytes[9] & 0x07);
			 */

			//sprintf(answer, "Pack Start (Mux Rate: %u Stuffing: %u)",
			// FIXME: too many damn bits
			//                                system_clock_ref_base,
			//                                system_clock_ref_ext,
			//program_mux_rate, pack_stuffing_length);
			//return answer;
		} else {
			//return "Pack Start (incomplete flags)";
		}
	} else if (code == PES_TYPE_group_start) {
		//if (buffer_look_ahead(bytes, 4) == 0) {
		gop_header_t gop;
		if (buffer_look_ahead((uint8_t *)&gop, sizeof(gop)) == 0) {
			gop.data = bswap(gop.data);
			/*
			   int drop = ((bytes[0] & 0x80) > 0);
			   int hour = ((bytes[0] & 0x7C) >> 2);
			   int min = ((bytes[0] & 0x3) << 4) | ((bytes[1] & 0xF0) >> 4);
			   int sec = ((bytes[1] & 0x7) << 3) | ((bytes[2] & 0xE0) >> 6);
			   int pictures = ((bytes[2] & 0x3F) << 1) | ((bytes[3] & 0x80) >> 7);
			   int closed = ((bytes[3] & 0x40) > 0);
			   int broken = ((bytes[3] & 0x20) > 0);
			 */

			/*
			   sprintf(answer, "  GOP Start (%02d:%02d:%02d.%02d%s%s%s)", hour, min, sec, pictures, drop ? " drop" : "", closed ? " closed" : " open", broken ? " broken" : "");
			 */

			gop.padding2 = gop.padding = gop.closed = gop.broken =
			    gop.drop = 0;
			last_index_written.frame = framecount;
			last_index_written.timestamp = gop;
			//((unsigned long long)((gop.hour * (60*60)) + (gop.minute*60) + gop.second) << 32LL) + gop.frame;
			last_index_written.offset = file_offset_of_last_packet;

			if (indexfd)
				fwrite(&last_index_written,
				       sizeof(last_index_written), 1, indexfd);

			last_was_gop_packet = 1;
			if (VERBOSE) {
				fprintf(stderr,
					"==> % 15lld: GOP 0x%08x (%02d:%02d:%02d.%02d) on frame %d; %s %s %s\n",
					(long long)file_offset_of_last_packet,
					(int)(last_index_written.timestamp.
					      data), gop.hour, gop.minute,
					gop.second, gop.frame, framecount,
					(gop.closed) ? "closed" : "",
					(gop.broken) ? "broken" : "",
					(gop.drop) ? "drop" : "");
			}
			/* Write out Status to LOCK */
			if (STATUS == 1 && bstatus != NULL) {
				FILE *lck = NULL;
				lck = fopen(bstatus, "w");
				if (lck != NULL) {
					/* Write PID to LOCK */
					fprintf(lck,
						"% 15lld: GOP 0x%08x %02d:%02d:%02d.%02d frame %d; %s %s %s\n",
						(long long)file_offset_of_last_packet,
						(int)(last_index_written.
						      timestamp.data), gop.hour,
						gop.minute, gop.second,
						gop.frame, framecount,
						(gop.closed) ? "closed" : "",
						(gop.broken) ? "broken" : "",
						(gop.drop) ? "drop" : "");
					fclose(lck);
				}
			}
		} else {
			//return "  GOP Start (incomplete flags)";
		}
	} else if (code == PES_TYPE_picture_start) {
		framecount++;
	}

	return position;
}

int mindex(char *i_mpeg, char *o_index, char *bstatus)
{
	uint32_t marker;
	uint8_t byte;

	int running = 1;

	if (i_mpeg == NULL || o_index == NULL) {
		fprintf(stderr,
			"mpegindex: Syntax is mpegindex mpegfile indexfile\n\n");
		return -1;
	}

	indexfd = fopen(o_index, "w");
	if (!indexfd) {
		fprintf(stderr,
			"mpegindex: Error: cannot create index file %s.\n",
			o_index);
		return -2;
	}

	if (buffer_start(i_mpeg) < 0) {
		fprintf(stderr, "mpegindex: Error: cannot open mpeg file %s.\n",
			i_mpeg);
		return -3;
	}

	buffer_seek(begin_at);

	marker = 0xFFFFFFFF;
	while (running) {
		//fprintf(stderr,"%08X\n",marker);
		if ((marker & 0xFFFFFF00) == 0x100) {

			loff_t newpos = process_packet(byte, bstatus);
			// we skipped to a new location?
			if (newpos != 0) {
				//fprintf(stdout,"skipping to %" OFF_T_FORMAT "\n",newpos);
				marker = 0xFFFFFFFF;
				buffer_seek(newpos);
			}
		}
		marker <<= 8;
		if (buffer_get_byte(&byte) < 0) {
			//running = 0;
		} else
			marker |= byte;

		if (num_bytes && buffer_tell() - begin_at > num_bytes) {
			//running = 0;
		}

		/*if (last_was_gop_packet && ((last_index_written.timestamp.second % 30) == 0)) {
		   fprintf(stdout, "\r%s: Processed frame %d at %s (file offset %lld)", 
		   i_mpeg, last_index_written.frame, timestamp_to_string(NULL, last_index_written.timestamp), last_index_written.offset);
		   fflush(stdout);
		   } */
	}

	fprintf(stdout,
		"\r%s: Processed %d frames covering time %s (file size %lld)\n\n",
		i_mpeg, last_index_written.frame, timestamp_to_string(NULL,
								      last_index_written.
								      timestamp),
		last_index_written.offset);

	fclose(indexfd);
	return 0;
}
