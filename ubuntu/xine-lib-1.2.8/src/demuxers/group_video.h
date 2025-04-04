/*
 * Copyright (C) 2000-2016 the xine project
 *
 * This file is part of xine, a free video player.
 *
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 */

#ifndef HAVE_GROUP_VIDEO_H
#define HAVE_GROUP_VIDEO_H

#include <xine/xine_internal.h>

void *demux_avi_init_class         (xine_t *xine, void *data);
void *demux_elem_init_class        (xine_t *xine, void *data);
void *demux_flv_init_class         (xine_t *xine, void *data);
void *demux_iff_init_class         (xine_t *xine, void *data);
void *demux_matroska_init_class    (xine_t *xine, void *data);
void *demux_mpeg_init_class        (xine_t *xine, void *data);
void *demux_pes_init_class         (xine_t *xine, void *data);
void *demux_mpeg_block_init_class  (xine_t *xine, void *data);
void *demux_qt_init_class          (xine_t *xine, void *data);
void *demux_rawdv_init_class       (xine_t *xine, void *data);
void *demux_real_init_class        (xine_t *xine, void *data);
void *demux_ts_init_class          (xine_t *xine, void *data);
void *demux_vc1es_init_class       (xine_t *xine, void *data);
void *demux_yuv_frames_init_class  (xine_t *xine, void *data);
void *demux_yuv4mpeg2_init_class   (xine_t *xine, void *data);

#endif

