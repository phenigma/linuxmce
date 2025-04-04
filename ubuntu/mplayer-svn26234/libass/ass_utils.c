// -*- c-basic-offset: 8; indent-tabs-mode: t -*-
// vim:ts=8:sw=8:noet:ai:
/*
  Copyright (C) 2006 Evgeniy Stepanov <eugeni.stepanov@gmail.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "config.h"

#include <stdlib.h>
#include <inttypes.h>
#include <ft2build.h>
#include FT_GLYPH_H

#include "mputils.h"
#include "ass_utils.h"

int mystrtoi(char** p, int base, int* res)
{
	char* start = *p;
	*res = strtol(*p, p, base);
	if (*p != start) return 1;
	else return 0;
}

int mystrtou32(char** p, int base, uint32_t* res)
{
	char* start = *p;
	*res = strtoll(*p, p, base);
	if (*p != start) return 1;
	else return 0;
}

int mystrtod(char** p, double* res)
{
	char* start = *p;
	*res = strtod(*p, p);
	if (*p != start) return 1;
	else return 0;
}

int strtocolor(char** q, uint32_t* res)
{
	uint32_t color = 0;
	int result;
	char* p = *q;
	
	if (*p == '&') ++p; 
	else mp_msg(MSGT_ASS, MSGL_DBG2, "suspicious color format: \"%s\"\n", p);
	
	if (*p == 'H' || *p == 'h') { 
		++p;
		result = mystrtou32(&p, 16, &color);
	} else {
		result = mystrtou32(&p, 0, &color);
	}
	
	{
		unsigned char* tmp = (unsigned char*)(&color);
		unsigned char b;
		b = tmp[0]; tmp[0] = tmp[3]; tmp[3] = b;
		b = tmp[1]; tmp[1] = tmp[2]; tmp[2] = b;
	}
	if (*p == '&') ++p;
	*q = p;

	*res = color;
	return result;
}

#if 0
static void sprint_tag(uint32_t tag, char* dst)
{
	dst[0] = (tag >> 24) & 0xFF;
	dst[1] = (tag >> 16) & 0xFF;
	dst[2] = (tag >> 8) & 0xFF;
	dst[3] = tag & 0xFF;
	dst[4] = 0;
}

void dump_glyph(FT_Glyph g)
{
	char tag[5];
	int i;
	FT_OutlineGlyph og = (FT_OutlineGlyph)g;
	FT_Outline* o = &(og->outline);
	sprint_tag(g->format, tag);
	printf("glyph: %p \n", g);
	printf("format: %s \n", tag);
	printf("outline: %p \n", o);
	printf("contours: %d, points: %d, points ptr: %p \n", o->n_contours, o->n_points, o->points);
	for (i = 0; i < o->n_points; ++i) {
		printf("  point %f, %f \n", d6_to_double(o->points[i].x), d6_to_double(o->points[i].y));
	}
}
#endif
