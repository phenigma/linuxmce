/*
    cx25840 API header
    Copyright (C) 2003-2004  Chris Kennedy

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _CX25840_H_
#define _CX25840_H_

#include "decoder.h"

struct cx25840_info {
        struct decoder_state state;
        int cardtype;
        int *layout;
	char *firmware;
	int fastfw;
};

#define CARD_TYPE_PVR150 0
#define CARD_TYPE_PG600  1

#define DECODER_GET_VIDEO_STATUS _IOR('d', 200, int *)
#define DECODER_SET_CARD_TYPE    _IOW('d', 201, int)

#endif
