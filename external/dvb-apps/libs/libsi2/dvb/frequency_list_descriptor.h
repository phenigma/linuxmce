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

#ifndef _SI_DVB_FREQUENCY_LIST_DESCRIPTOR
#define _SI_DVB_FREQUENCY_LIST_DESCRIPTOR 1

#include <si/descriptor.h>
#include <si/common.h>

struct dvb_frequency_list_descriptor {
	struct descriptor d;

  EBIT2(uint8_t reserved	: 6; ,
	uint8_t coding_type	: 2; );
	/* uint32_t centre_frequencies [] */
} packed;

static inline struct dvb_frequency_list_descriptor*
	dvb_frequency_list_descriptor_parse(struct descriptor* d)
{
	int pos = 0;
	uint8_t* buf = (uint8_t*) d + 2;
	int len = d->len;

	pos += sizeof(struct dvb_frequency_list_descriptor) - 2;

	if ((len - pos) % 4)
		return NULL;

	while(pos < len) {
		bswap32(buf+pos);
		pos += 4;
	}

	return (struct dvb_frequency_list_descriptor*) d;
}

static inline uint32_t *
	dvb_frequency_list_descriptor_centre_frequencies(struct dvb_frequency_list_descriptor *d)
{
	return (uint32_t *) ((uint8_t *) d + sizeof(struct dvb_frequency_list_descriptor));
}

static inline int
	dvb_frequency_list_descriptor_centre_frequencies_count(struct dvb_frequency_list_descriptor *d)
{
	return (d->d.len - 1) >> 2;
}

#endif
