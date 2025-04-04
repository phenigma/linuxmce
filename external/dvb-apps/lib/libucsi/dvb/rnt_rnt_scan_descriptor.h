/*
 * section and descriptor parser
 *
 * Copyright (C) 2005 Kenneth Aafloy (kenneth@linuxtv.org)
 * Copyright (C) 2005 Andrew de Quincey (adq_dvb@lidskialf.net)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef _UCSI_DVB_RNT_RNT_SCAN_DESCRIPTOR
#define _UCSI_DVB_RNT_RNT_SCAN_DESCRIPTOR 1

#ifdef __cplusplus
extern "C"
{
#endif

#include <ucsi/descriptor.h>
#include <ucsi/endianops.h>
#include <ucsi/types.h>

/**
 * dvb_rnt_rnt_scan_descriptor structure.
 */
struct dvb_rnt_rnt_scan_descriptor {
	struct descriptor d;

	/* struct dvb_rnt_rnt_scan_entry entries[] */
} packed;

/**
 * An entry in the entries field of a dvb_rnt_rnt_scan_descriptor.
 */
struct dvb_rnt_rnt_scan_entry {
	uint16_t transport_stream_id;
	uint16_t original_network_id;
	uint8_t scan_weighting;
} packed;

/**
 * Process a dvb_rnt_rnt_scan_descriptor.
 *
 * @param d Generic descriptor.
 * @return dvb_rnt_rnt_scan_descriptor pointer, or NULL on error.
 */
static inline struct dvb_rnt_rnt_scan_descriptor*
	dvb_rnt_rnt_scan_descriptor_codec(struct descriptor* d)
{
	uint8_t *buf = (uint8_t*) d;
	int len = d->len +2;
	int pos = 2;

	if ((len-2) % sizeof(struct dvb_rnt_rnt_scan_entry))
		return NULL;

	while(pos < len) {
		bswap16(buf+pos);
		bswap16(buf+pos+2);
		pos += sizeof(struct dvb_rnt_rnt_scan_entry);
	}

	return (struct dvb_rnt_rnt_scan_descriptor*) d;
}

/**
 * Iterator for entries field of a dvb_rnt_rnt_scan_descriptor.
 *
 * @param d dvb_rnt_rnt_scan_descriptor pointer.
 * @param pos Variable holding a pointer to the current dvb_rnt_rnt_scan_entry.
 */
#define dvb_rnt_rnt_scan_descriptor_entries_for_each(d, pos) \
	for ((pos) = dvb_rnt_rnt_scan_descriptor_entries_first(d); \
	     (pos); \
	     (pos) = dvb_rnt_rnt_scan_descriptor_entries_next(d, pos))










/******************************** PRIVATE CODE ********************************/
static inline struct dvb_rnt_rnt_scan_entry*
	dvb_rnt_rnt_scan_descriptor_entries_first(struct dvb_rnt_rnt_scan_descriptor *d)
{
	if (d->d.len == 0)
		return NULL;

	return (struct dvb_rnt_rnt_scan_entry *)
		((uint8_t*) d + sizeof(struct dvb_rnt_rnt_scan_descriptor));
}

static inline struct dvb_rnt_rnt_scan_entry*
	dvb_rnt_rnt_scan_descriptor_entries_next(struct dvb_rnt_rnt_scan_descriptor *d,
					     struct dvb_rnt_rnt_scan_entry *pos)
{
	uint8_t *end = (uint8_t*) d + 2 + d->d.len;
	uint8_t *next =	(uint8_t *) pos + sizeof(struct dvb_rnt_rnt_scan_entry);

	if (next >= end)
		return NULL;

	return (struct dvb_rnt_rnt_scan_entry *) next;
}

#ifdef __cplusplus
}
#endif

#endif
