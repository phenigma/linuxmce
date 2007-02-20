/*
#
# MPEG2 data structures for MPEG2Parser class
#
# $Id: mpeg2structs.h,v 1.2 2003/11/01 08:35:37 knup Exp $
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

#ifndef _MPEG2STRUCTS_H_
#define _MPEG2STRUCTS_H_

#ifndef TRUE
# define TRUE 1
#endif
#ifndef FALSE
# define FALSE 0
#endif

/*
   This problem with these damn compilers is that I can't
   specify an arbitrary bit layout structure.  It always
   grows to a different size.  If someone know how to portably
   control that problem let me know, and I can stop using bit masks
   to get at my data.  :)
*/

#define PICTURE_CODING_MASK		0x38
#define PICTURE_CODING_SHIFT		0x03
#define PICTURE_CODING_INTRA		0x01
#define PICTURE_CODING_PREDICTIVE	0x02
#define PICTURE_CODING_BIDIRECTIONAL	0x03

typedef struct {
	uint8_t start_code[4];
	uint8_t bits[2];
	/*  only stuff I want to get to is picture_coding_type
	   uint16_t temporal_reference:10;
	   uint8_t  picture_coding_type:3;
	 */
} picture_header;

#define GOP_FLAG_MASK	0x60
#define GOP_FLAG_CLOSED	0x40
#define GOP_FLAG_BROKEN	0x20

typedef struct {
	uint8_t start_code[4];
	uint8_t bits[4];
	/*
	   uint32_t time_code:25;
	   uint8_t  closed_gop:1;
	   uint8_t  broken_link:1;
	   uint8_t  unknown:5;
	 */
} group_of_pictures_header;

#define ES_TYPE_WEIRD   0
#define ES_TYPE_VIDEO   1
#define ES_TYPE_AUDIO   2

typedef struct {
	uint8_t start_code[3];
	uint8_t stream_id;
	uint8_t PES_packet_length[2];
} PES_packet_header_t;

typedef struct {
	uint8_t bits[2];
	/*
	   uint8_t         marker0:2; // 10
	   uint8_t         PES_scrambling_control:2;
	   uint8_t         PES_priority:1;
	   uint8_t         data_alignment_indicator:1;
	   uint8_t         copyright:1;
	   uint8_t         original_or_copy:1;

	   uint8_t         PTS_DTS_flags:2;
	   uint8_t         ESCR_flag:1;
	   uint8_t         ES_rate_flag:1;
	   uint8_t         DSM_trick_mode_flag:1;
	   uint8_t         additional_copy_info_flag:1;
	   uint8_t         PES_CRC_flag:1;
	   uint8_t         PES_extension_flag:1;
	 */
	uint8_t PES_header_data_length;
} PES_packet_internals_t;

typedef struct {
	uint8_t marker_bit:1;
	uint8_t data:7;
} PES_packet_additional_copy_info_t;

#define GET_BITS(byte,mask,shiftdown)	(((byte)&(mask))>>(shiftdown))

#define PPE_PES_private_data_flag(x)	GET_BITS(x,0x80,7)
#define PPE_pack_header_field_flag(x)	GET_BITS(x,0x40,6)
#define PPE_program_packet_sequence_counter_flag(x)	GET_BITS(x,0x20,5)
#define PPE_P_STD_buffer_flag(x)		GET_BITS(x,0x10,5)
#define PPE_PES_extension_flag_2(x)		GET_BITS(x,0x01,0)
typedef struct {
	uint8_t bits[1];
	/*
	   uint8_t         PES_private_data_flag:1;
	   uint8_t         pack_header_field_flag:1;
	   uint8_t         program_packet_sequence_counter_flag:1;
	   uint8_t         P_STD_buffer_flag:1;
	   uint8_t         reserved:3;
	   uint8_t         PES_extension_flag_2:1;
	 */
} PES_packet_extension_t;

typedef struct {
	uint8_t marker0:1;
	uint8_t program_packet_sequence_counter:7;

	uint8_t marker1:1;
	uint8_t MPEG1_MPEG2_identifier:1;
	uint8_t original_stuff_length:6;
} PES_packet_extension_program_packet_sequence_counter_t;

#define PES_TYPE_picture_start		0x00
// slice start: 0x01 - 0xAF
// reserved: 0xB0 - 0xB1
#define PES_TYPE_user_data_start	0xB2
#define PES_TYPE_sequence_header	0xB3
#define PES_TYPE_sequence_error		0xB4
#define PES_TYPE_extension_start	0xB5
// reserved: 0xB6
#define PES_TYPE_sequence_end		0xB7
#define PES_TYPE_group_start		0xB8
#define PES_TYPE_program_end		0xB9
#define PES_TYPE_pack_start		0xBA
#define PES_TYPE_system_header		0xBB
#define PES_TYPE_program_stream_map     0xBC	/* PES simple */
#define PES_TYPE_private_stream_1       0xBD	/* PES complex */
#define PES_TYPE_padding_stream         0xBE	/* PES simple */
#define PES_TYPE_private_stream_2       0xBF	/* PES simple */
#define PES_TYPE_ECM_stream             0xF0	/* PES simple */
#define PES_TYPE_EMM_stream             0xF1	/* PES simple */
#define PES_TYPE_DSMCC_stream           0xF2	/* PES simple */
#define PES_TYPE_ISO_13522              0xF3	/* PES complex */
#define PES_TYPE_H2221A                 0xF4	/* PES complex */
#define PES_TYPE_H2221B                 0xF5	/* PES complex */
#define PES_TYPE_H2221C                 0xF6	/* PES complex */
#define PES_TYPE_H2221D                 0xF7	/* PES complex */
#define PES_TYPE_H2221E                 0xF8	/* PES simple */
#define PES_TYPE_ancillary_stream       0xF9	/* PES complex */
// reserved: 0xFA - 0xFE                        /* PES complex */
#define PES_TYPE_program_stream_directory       0xFF	/* PES simple */

#define PES_TYPE_audio                  0xC0	/* PES complex */
#define PES_TYPE_MASK_audio             0xE0

#define PES_TYPE_video                  0xE0	/* PES complex */
#define PES_TYPE_MASK_video             0xF0

//                            xxxxx     xxxxx
// subtitles: 0x20..0x3F   00100000..00111111
#define DVD_AUDIO_TYPE_sub              0x20
#define DVD_AUDIO_TYPE_MASK_sub         0xE0
//                            xxxxx     xxxxx
// AC3:       0x80..0x9F   10000000..10011111
#define DVD_AUDIO_TYPE_ac3              0x80
#define DVD_AUDIO_TYPE_MASK_ac3         0xE0
//                            xxxxx     xxxxx 
// PCM:       0xA0..0xBF   10100000..10111111
#define DVD_AUDIO_TYPE_pcm              0xA0
#define DVD_AUDIO_TYPE_MASK_pcm         0xE0

typedef struct {
	uint8_t start_code[4];
	uint16_t header_length;

	uint8_t bits[6];
	/*
	   uint32_t        marker0:1; // 1
	   uint32_t        rate_bound:22;
	   uint32_t        marker1:1; // 1
	   uint32_t        audio_bound:6;
	   uint32_t        fixed_flag:1;
	   uint32_t        CSPS_flag:1;

	   uint8_t         system_audio_lock_flag:1;
	   uint8_t         system_video_lock_flag:1;
	   uint8_t         marker2:1; // 1
	   uint8_t         video_bound:5;

	   uint8_t         packet_rate_restriction_flag:1;
	   uint8_t         reserved:7;
	 */

} system_header_t;

typedef struct {
	uint8_t stream_id;
	// 1011 1000: P_STD map to all audio streams
	// 1011 1001: P_STD map to all video streams
	// otherwise, must be >= 1011 1100
	//
	uint8_t bits[2];
	/*
	   uint16_t        marker0:2; // 11
	   uint16_t        P_STD_buffer_bound_scale:1;
	   uint16_t        P_STD_buffer_size_bound:13;
	 */
} stream_id_t;

typedef struct {
	uint8_t start_code[4];
	/*
	   uint32_t        marker0:2; // 01
	   uint32_t        system_clock_reference_base_hi:3;
	   uint32_t        marker1:1; // 1
	   uint32_t        system_clock_reference_base_mid:15;
	   uint32_t        marker2:1; // 1
	   uint32_t        system_clock_reference_base_low:15;
	   uint16_t        marker3:1; // 1
	   uint16_t        system_clock_reference_extension:9;
	   uint16_t        marker4:1; // 1
	   uint32_t        program_mux_rate:22;
	   uint32_t        marker5:1; // 1
	   uint32_t        marker6:1; // 1
	   uint8_t reserved:5;
	   uint8_t pack_stuffing_length:3;
	 */
	uint8_t bits[10];
} pack_header_t;

#endif				//  _MPEG2STRUCTS_H_

/* vi:set ai ts=4 sw=4 expandtab: */
