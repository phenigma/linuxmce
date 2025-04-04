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

#ifndef _SI_MPEG_ISO_639_LANGUAGE_DESCRIPTOR
#define _SI_MPEG_ISO_639_LANGUAGE_DESCRIPTOR 1

#include <si/descriptor.h>
#include <si/common.h>

struct mpeg_iso_639_language_descriptor {
	struct descriptor d;

	/* struct mpeg_iso_639_language_code languages[] */
} packed;

struct mpeg_iso_639_language_code {
	uint8_t iso_639_language_code[3];
	uint8_t audio_type;
} packed;

static inline struct mpeg_iso_639_language_descriptor*
	mpeg_iso_639_language_descriptor_parse(struct descriptor* d)
{
	if (d->len % sizeof(struct mpeg_iso_639_language_code))
		return NULL;

	return (struct mpeg_iso_639_language_descriptor*) d;
}

#define mpeg_iso_639_language_descriptor_languages_for_each(d, pos) \
	for ((pos) = mpeg_iso_639_language_descriptor_languages_first(d); \
	     (pos); \
	     (pos) = mpeg_iso_639_language_descriptor_languages_next(d, pos))










/******************************** PRIVATE CODE ********************************/
static inline struct mpeg_iso_639_language_code*
	mpeg_iso_639_language_descriptor_languages_first(struct mpeg_iso_639_language_descriptor *d)
{
	if (d->d.len < sizeof(struct mpeg_iso_639_language_code))
		return NULL;

	return (struct mpeg_iso_639_language_code *)
		(uint8_t*) d + sizeof(struct mpeg_iso_639_language_descriptor);
}

static inline struct mpeg_iso_639_language_code*
	mpeg_iso_639_language_descriptor_languages_next(struct mpeg_iso_639_language_descriptor *d,
						        struct mpeg_iso_639_language_code *pos)
{
	uint8_t *end = (uint8_t*) d + 2 + d->d.len;
	uint8_t *next =	(uint8_t *) pos + sizeof(struct mpeg_iso_639_language_code);

	if (next >= end)
		return NULL;

	return (struct mpeg_iso_639_language_code *) next;
}

#endif
