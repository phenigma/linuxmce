/*
    Vertical Blank Interval support functions
    Copyright (C) 2004  Hans Verkuil <hverkuil@xs4all.nl>

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

#include "ivtv-driver.h"
#include "ivtv-video.h"
#include "ivtv-i2c.h"
#include "ivtv-vbi.h"

/* This structure is used when reading from /dev/vbi8 */
struct ivtv_sliced_vbi_data {
	unsigned long id;
	unsigned long line;
	unsigned char data[44];
};

/* If the VBI slicer does not detect any signal it will fill the 42 byte
   buffer with 0xa0. */
static const char vbi_no_data[] = {
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0xa0, 0xa0
};

static int has_vbi_data(const u8 *vbi)
{
	// skip if this line contains no data
	if (!memcmp(vbi, vbi_no_data, 42)) {
		return 0;
	}
	return 1;
}

static int odd_parity(u8 c)
{
	c ^= (c >> 4);
	c ^= (c >> 2);
	c ^= (c >> 1);

	return c & 1;
}

static void vbi_schedule_work(struct ivtv *itv)
{
#ifdef LINUX26
	queue_work(itv->vbi_work_queues, &itv->vbi_work_queue);
#else
	ivtv_schedule_work(&itv->vbi_work_queue);
#endif
}

static int decode_vps(u8 *dst, u8 *p)
{
	static u8 biphase_tbl[] = {
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4, 
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0, 
		0xd2, 0x5a, 0x52, 0xd2, 0x96, 0x1e, 0x16, 0x96, 
		0x92, 0x1a, 0x12, 0x92, 0xd2, 0x5a, 0x52, 0xd2, 
		0xd0, 0x58, 0x50, 0xd0, 0x94, 0x1c, 0x14, 0x94, 
		0x90, 0x18, 0x10, 0x90, 0xd0, 0x58, 0x50, 0xd0, 
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4, 
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0, 
		0xe1, 0x69, 0x61, 0xe1, 0xa5, 0x2d, 0x25, 0xa5, 
		0xa1, 0x29, 0x21, 0xa1, 0xe1, 0x69, 0x61, 0xe1, 
		0xc3, 0x4b, 0x43, 0xc3, 0x87, 0x0f, 0x07, 0x87, 
		0x83, 0x0b, 0x03, 0x83, 0xc3, 0x4b, 0x43, 0xc3, 
		0xc1, 0x49, 0x41, 0xc1, 0x85, 0x0d, 0x05, 0x85, 
		0x81, 0x09, 0x01, 0x81, 0xc1, 0x49, 0x41, 0xc1, 
		0xe1, 0x69, 0x61, 0xe1, 0xa5, 0x2d, 0x25, 0xa5, 
		0xa1, 0x29, 0x21, 0xa1, 0xe1, 0x69, 0x61, 0xe1, 
		0xe0, 0x68, 0x60, 0xe0, 0xa4, 0x2c, 0x24, 0xa4, 
		0xa0, 0x28, 0x20, 0xa0, 0xe0, 0x68, 0x60, 0xe0, 
		0xc2, 0x4a, 0x42, 0xc2, 0x86, 0x0e, 0x06, 0x86, 
		0x82, 0x0a, 0x02, 0x82, 0xc2, 0x4a, 0x42, 0xc2, 
		0xc0, 0x48, 0x40, 0xc0, 0x84, 0x0c, 0x04, 0x84, 
		0x80, 0x08, 0x00, 0x80, 0xc0, 0x48, 0x40, 0xc0, 
		0xe0, 0x68, 0x60, 0xe0, 0xa4, 0x2c, 0x24, 0xa4, 
		0xa0, 0x28, 0x20, 0xa0, 0xe0, 0x68, 0x60, 0xe0, 
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4, 
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0, 
		0xd2, 0x5a, 0x52, 0xd2, 0x96, 0x1e, 0x16, 0x96, 
		0x92, 0x1a, 0x12, 0x92, 0xd2, 0x5a, 0x52, 0xd2, 
		0xd0, 0x58, 0x50, 0xd0, 0x94, 0x1c, 0x14, 0x94, 
		0x90, 0x18, 0x10, 0x90, 0xd0, 0x58, 0x50, 0xd0, 
		0xf0, 0x78, 0x70, 0xf0, 0xb4, 0x3c, 0x34, 0xb4, 
		0xb0, 0x38, 0x30, 0xb0, 0xf0, 0x78, 0x70, 0xf0, 
	};
	int i;
	u8 c, err = 0;

	for (i = 0; i < 2 * 13; i += 2) {
		err |= biphase_tbl[p[i]] | biphase_tbl[p[i + 1]];
		c = (biphase_tbl[p[i + 1]] & 0xf) | ((biphase_tbl[p[i]] & 0xf) << 4);
		dst[i / 2] = c;
	}
	return err & 0xf0;
}

static int decode_wss(u8 *p)
{
	static const int wss_bits[8] = {
		0, 0, 0, 1, 0, 1, 1, 1
	};
	unsigned char parity;
	int wss = 0;
	int i;

	for (i = 0; i < 16; i++) {
		int b1 = wss_bits[p[i] & 7];
		int b2 = wss_bits[(p[i] >> 3) & 7];

		if (b1 == b2) return -1;
		wss |= b2 << i;
	}
	parity = wss & 15;
	parity ^= parity >> 2;
	parity ^= parity >> 1;

	if (!(parity & 1)) return -1;

	return wss;
}

static void passthrough_wss(struct ivtv *itv, int wss)
{
	if (wss < 0) 
		return;
	itv->vbi_wss = wss;
	itv->vbi_wss_found = 1;
}

static void passthrough_vbi_data(struct ivtv *itv, u8 *p, int cnt)
{
	long linemask[2] = { 0, 0 };
	int i;
	u8 vps[13];

        if (!memcmp(p, "itv0", 4)) {
            memcpy(linemask, p + 4, 8);
            p += 12;
        }
        else if (memcmp(p, "ITV0", 4)) {
            linemask[0] = 0xffffffff;
            linemask[1] = 0xf;
            p += 4;
        }
        else {
            /* unknown VBI data stream */
            return;
        }
	itv->vbi_vps_found = itv->vbi_wss_found = 0;
	itv->vbi_cc_mode = 0;
	itv->vbi_wss = 0;

	for (i = 0; i < 36; i++) {
		if (i < 32 && !(linemask[0] & (1 << i))) continue;
		if (i >= 32 && !(linemask[1] & (1 << (i - 32)))) continue;
		switch (*p & 0xf) {
			case VBI_TYPE_CC:
				if (itv->vbi_passthrough & (IVTV_SLICED_CAPTION_625 | IVTV_SLICED_CAPTION_525)) {
					if (i >= 18) {
						itv->vbi_cc_mode |= 2;
						itv->vbi_cc_data[2] = p[1];
						itv->vbi_cc_data[3] = p[2];
					}
					else {
						itv->vbi_cc_mode |= 1;
						itv->vbi_cc_data[0] = p[1];
						itv->vbi_cc_data[1] = p[2];
					}
				}
				break;
			case VBI_TYPE_VPS:
				if (itv->vbi_passthrough & IVTV_SLICED_VPS) {
					if (decode_vps(vps, p + 1) == 0) {
						itv->vbi_vps[0] = vps[2];
						itv->vbi_vps[1] = vps[8];
						itv->vbi_vps[2] = vps[9];
						itv->vbi_vps[3] = vps[10];
						itv->vbi_vps[4] = vps[11];
						itv->vbi_vps_found = 1;
					}
				}
				break;
			case VBI_TYPE_WSS:
				if (itv->vbi_passthrough & IVTV_SLICED_WSS_625) {
					passthrough_wss(itv, decode_wss(p + 1));
				}
				break;
			default:
				break;
		}
		p += 43;
	}
	vbi_schedule_work(itv);
}

static void copy_vbi_data(struct ivtv *itv, u8 *p, int cnt, u32 pts_stamp)
{
	int line = 0;
	long linemask[2] = { 0, 0 };
	unsigned short size;
	static u8 mpeg_hdr_data[] = {
		0x00, 0x00, 0x01, 0xbd, 0x00, 0x1a, 0x84, 0x80,
		0x07, 0x21, 0x00, 0x5d, 0x63, 0xa7, 0xff, 0xff
	};
	const int sd = 17;  /* start of vbi data */
	int idx = itv->vbi_frame % 20;
	u8 *dst = &itv->vbi_sliced_mpeg_data[idx][0];
	int invert = (itv->std & V4L2_STD_525_60); // The field bit is inverted for NTSC

	while (cnt-- > 0) {
		if (*p++ == 0xff && (p[2] == 0xAB || p[2] == 0xEC) && cnt > 48) {
			int id1, id2, l;

			p += 3; cnt -= 3;
			id1 = p[2];
			if (invert) id1 ^= 0x40;
			id2 = p[3];
			p += 4; cnt -= 4;
			l = (id1 & 0x3f) << 3;
			l |= (id2 & 0x70) >> 4;
			id2 &= 0xf;
			dst[sd + 12 + line * 43] = id2;
			if (id1 & 0x40) l += 18;
			l -= 6;
			if (has_vbi_data(p) && l >= 0 && l < 36) {
				if (l < 32) linemask[0] |= (1 << l);
				else linemask[1] |= (1 << (l - 32));
				memcpy(dst + sd + 12 + line * 43 + 1, p, 42);
				line++;
			}
			p += 40;
			cnt -= 40;
		}
	}
	memcpy(dst + 1, mpeg_hdr_data, sizeof(mpeg_hdr_data));
        if (line == 36) {
            /* All lines are used, so there is no space for the linemask
               (the max size of the VBI data is 36 * 43 + 4 bytes).
               So in this case we use the magic number 'ITV0'. */
            memcpy(dst + sd, "ITV0", 4);
            memcpy(dst + sd + 4, dst + sd + 12, line * 43);
	    size = 10 + 4 + ((43 * line + 3) & ~3);
        }
        else {
            memcpy(dst + sd, "itv0", 4);
	    memcpy(dst + sd + 4, &linemask[0], 8);
	    size = 10 + 12 + ((43 * line + 3) & ~3);
        }
	dst[5] = size >> 8;
	dst[6] = size & 0xff;
	dst[10] = 0x21 | ((pts_stamp >> 29) & 0x6);
	dst[11] = (pts_stamp >> 22) & 0xff;
	dst[12] = 1 | ((pts_stamp >> 14) & 0xff);
	dst[13] = (pts_stamp >> 7) & 0xff;
	dst[14] = 1 | ((pts_stamp & 0x7f) << 1);
	itv->vbi_sliced_mpeg_offset[idx] = 0;
	itv->vbi_sliced_mpeg_size[idx] = 7 + size;
}


int ivtv_DEC_VBI_fixup(struct ivtv *itv, u8 *p, int cnt, int field_lines)
{
	long linemask[2];
	int i, l, id2, wss;
	int line = 0;

        if (!memcmp(p, "itv0", 4)) {
            memcpy(linemask, p + 4, 8);
            p += 12;
        }
        else if (memcmp(p, "ITV0", 4)) {
            linemask[0] = 0xffffffff;
            linemask[1] = 0xf;
            p += 4;
        }
        else {
            /* unknown VBI data stream */
            return 0;
        }
	for (i = 0; i < 36; i++) {
		int err = 0;

		if (i < 32 && !(linemask[0] & (1 << i))) continue;
		if (i >= 32 && !(linemask[1] & (1 << (i - 32)))) continue;
		id2 = *p & 0xf;
		switch (id2) {
			case VBI_TYPE_TELETEXT:
				id2 = IVTV_SLICED_TELETEXT_B;
				break;
			case VBI_TYPE_CC:
				id2 = IVTV_SLICED_CAPTION_525;
				err = !odd_parity(p[1]) || !odd_parity(p[2]);
				break;
			case VBI_TYPE_VPS:
				id2 = IVTV_SLICED_VPS;
				err = decode_vps(p + 1, p + 1);
				break;
			case VBI_TYPE_WSS:
				id2 = IVTV_SLICED_WSS_625;
				wss = decode_wss(p + 1);
				err = (wss < 0);
				p[1] = wss & 0xff;
				p[2] = wss >> 8;
				break;
			default:
				id2 = 0;
				break;
		}
		if (err == 0) {
			l = (i < 18) ? i + 6 : i - 18 + 6 + field_lines;
			itv->vbi_sliced_dec_data[line].line = l;
			itv->vbi_sliced_dec_data[line].id = id2;
			memcpy(itv->vbi_sliced_dec_data[line].data, p + 1, 42);
			line++;
		}
		p += 43;
	}
	while (line < 36) {
		itv->vbi_sliced_dec_data[line].id = 0;
		itv->vbi_sliced_dec_data[line].line = 0;
		line++;
	}
	return line * sizeof(itv->vbi_sliced_dec_data[0]);
}

int ivtv_SLICED_VBI_fixup(struct ivtv *itv, u8 *p, int cnt, int field_lines)
{
	int line = 0;
	int wss;
	int invert = (itv->std & V4L2_STD_525_60); // The field bit is inverted for NTSC

	while (cnt-- > 0) {
		if (*p++ == 0xff && (p[2] == 0xAB || p[2] == 0xEC) && cnt > 48) {
			int id1, id2, l, err = 0;

			p += 3; cnt -= 3;
			id1 = p[2];
			if (invert) id1 ^= 0x40;
			id2 = p[3];
			p += 4; cnt -= 4;
			l = (id1 & 0x3f) << 3;
			l |= (id2 & 0x70) >> 4;
			id2 &= 0xf;
			if (!has_vbi_data(p)) {
				p += 40;
				cnt -= 40;
				continue;
			}
			switch (id2) {
				case VBI_TYPE_TELETEXT:
					id2 = IVTV_SLICED_TELETEXT_B;
					break;
				case VBI_TYPE_CC:
					id2 = IVTV_SLICED_CAPTION_525;
					err = !odd_parity(p[0]) || !odd_parity(p[1]);
					break;
				case VBI_TYPE_VPS:
					id2 = IVTV_SLICED_VPS;
					if (decode_vps(p, p) != 0) {
						err = 1;
					}
					break;
				case VBI_TYPE_WSS:
					id2 = IVTV_SLICED_WSS_625;
					wss = decode_wss(p);
					if (wss == -1) {
						err = 1;
					} else {
						p[0] = wss & 0xff;
						p[1] = wss >> 8;
					}
					break;
				default:
					id2 = 0;
					err = 1;
					break;
			}
			itv->vbi_sliced_data[line].id = id2;
			itv->vbi_sliced_data[line].line = l;
			if (id1 & 0x40) itv->vbi_sliced_data[line].line += field_lines;
			memcpy(itv->vbi_sliced_data[line].data, p, 42);
			p += 40;
			cnt -= 40;
			if (!err) line++;
		}
	}
	if (line == 0) {
		itv->vbi_sliced_data[0].id = 0;
		itv->vbi_sliced_data[0].line = 0;
		line = 1;
	}
	return line * sizeof(itv->vbi_sliced_data[0]);
}


ssize_t ivtv_write_vbi(struct ivtv *itv, const char *ubuf, size_t count)
{
	struct ivtv_sliced_vbi_data *p = (struct ivtv_sliced_vbi_data *)ubuf;

	if (itv->vbi_service_set_out == 0) return -EPERM;
	while (count >= sizeof(struct ivtv_sliced_vbi_data)) {
		switch (p->id) {
			case IVTV_SLICED_CAPTION_525:
			case IVTV_SLICED_CAPTION_625:
				if (p->id == IVTV_SLICED_CAPTION_525 &&
					(itv->vbi_service_set_out & IVTV_SLICED_CAPTION_525) == 0) {
					break;
				}
				if (p->id == IVTV_SLICED_CAPTION_625 &&
					(itv->vbi_service_set_out & IVTV_SLICED_CAPTION_625) == 0) {
					break;
				}
				if (p->line >= 200) {
					itv->vbi_cc_mode |= 2;
					itv->vbi_cc_data[2] = p->data[0];
					itv->vbi_cc_data[3] = p->data[1];
				}
				else {
					itv->vbi_cc_mode |= 1;
					itv->vbi_cc_data[0] = p->data[0];
					itv->vbi_cc_data[1] = p->data[1];
				}
				break;
			case IVTV_SLICED_VPS:
				if (itv->vbi_service_set_out & IVTV_SLICED_VPS) {
					itv->vbi_vps[0] = p->data[2];
					itv->vbi_vps[1] = p->data[8];
					itv->vbi_vps[2] = p->data[9];
					itv->vbi_vps[3] = p->data[10];
					itv->vbi_vps[4] = p->data[11];
					itv->vbi_vps_found = 1;
				}
				break;
			case IVTV_SLICED_WSS_625:
				if (itv->vbi_service_set_out & IVTV_SLICED_WSS_625) {
					itv->vbi_wss = p->data[0] | (p->data[1] << 8);
					itv->vbi_wss_found = 1;
				}
				break;
			default:
				break;
		}
		count -= sizeof(*p);
		p++;
	}
	vbi_schedule_work(itv);
	return (char *)p - ubuf;
}

void ivtv_process_vbi_data(struct ivtv *itv, struct ivtv_buffer *buf, 
		u32 pts_stamp, int streamtype)
{
	u8 *p = (u8 *)buf->buffer.m.userptr;
	u32 size = buf->buffer.bytesused;

	if (streamtype == IVTV_ENC_STREAM_TYPE_VBI &&
		itv->vbi_service_set_in == 0) {
		int i;

		/* Dirty hack needed for backwards compatibility of 
		   old VBI software. */
		memcpy(p + size - 4, &itv->vbi_frame, 4);

		/* The first few bytes of a VBI line contain rubbish
		   (start codes from the saa7115 chip). This confuses
		   some programs that use these bytes to determine
		   the min and max byte values. Replace these bytes
		   with a valid byte. */
		for (i = 0; i < size; i += VBI_RAW_SIZE) {
			int j;

			for (j = 0; j < 8; j++) {
				p[i + j] = p[i + 8];
			}
		}
	}
	else if (streamtype == IVTV_ENC_STREAM_TYPE_VBI &&
		itv->vbi_insert_mpeg) {
		copy_vbi_data(itv, p, size, pts_stamp);
	}
	else if (streamtype == IVTV_DEC_STREAM_TYPE_VBI &&
		itv->vbi_passthrough) {
		passthrough_vbi_data(itv, p, size);
	}
}

int ivtv_used_line(struct ivtv *itv, int line, int field)
{
	int set = itv->vbi_service_set_in;

	if (set & IVTV_SLICED_CAPTION_625) {
		if (line == 22) return 1;
	}
	if (set & IVTV_SLICED_CAPTION_525) {
		if (line == 21) return 1;
	}
	if (set & IVTV_SLICED_WSS_625) {
		if (line == 23 && field == 0) return 1;
	}
	if (set & IVTV_SLICED_VPS) {
		if (line == 16 && field == 0) return 1;
	}
	if (set & IVTV_SLICED_TELETEXT_B) {
		if (line >= 6 && line <= 22) return 1;
	}
	if (set == 0) {
		if (itv->std & V4L2_STD_625_50) {
			return line >= 6 && line <= 23;
		}
		return line >= 10 && line <= 21;
	}
	return 0;
}

int ivtv_calc_sliced_lines(int set)
{
	int lines = 0;

	if (set & IVTV_SLICED_CAPTION_625) {
		lines += 2;
	}
	if (set & IVTV_SLICED_CAPTION_525) {
		lines += 2;
	}
	if (set & IVTV_SLICED_WSS_625) {
		lines++;
	}
	if (set & IVTV_SLICED_VPS) {
		lines++;
	}
	if (set & IVTV_SLICED_TELETEXT_B) {
		lines = 34;
		if (set & IVTV_SLICED_WSS_625) {
			lines++;
		}
	}
	return lines;
}

void ivtv_disable_vbi(struct ivtv *itv)
{
	ivtv_set_wss(itv, 0, 0);
	ivtv_set_cc(itv, 0, 0, 0, 0, 0);
	ivtv_set_vps(itv, 0, 0, 0, 0, 0, 0);
	itv->vbi_vps_found = itv->vbi_wss_found = 0;
	itv->vbi_wss = 0;
	itv->vbi_cc_mode = 0;
}

void ivtv_start_vbi_timer(struct ivtv *itv)
{
	if (itv->std & V4L2_STD_525_60) {
		itv->vbi_passthrough_timer.expires = jiffies + HZ / 60;
	} else {
		itv->vbi_passthrough_timer.expires = jiffies + HZ / 50;
	}
	add_timer(&itv->vbi_passthrough_timer);
}

void ivtv_set_vbi(unsigned long arg)
{
	struct ivtv *itv = (struct ivtv *)arg;

	if (!test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags)) 
		return;

	ivtv_start_vbi_timer(itv);
	vbi_schedule_work(itv);
}

void vbi_work_handler(void *arg)
{
	struct ivtv *itv = arg;
		
	if (test_bit(IVTV_F_I_PASSTHROUGH, &itv->i_flags)) {
		if (itv->vbi_passthrough & IVTV_SLICED_WSS_625) {
			int wss;

			ivtv_saa7115(itv, DECODER_GET_WSS, &wss);
			if (wss >= 0) {
				ivtv_set_wss(itv, 1, wss & 7);
				itv->vbi_wss_no_update = 0;
			} else if (itv->vbi_wss_no_update == 4) {
				ivtv_set_wss(itv, 1, 0);
			} else {
				itv->vbi_wss_no_update++;
			}
		}
		if (itv->vbi_passthrough & (IVTV_SLICED_CAPTION_625 | IVTV_SLICED_CAPTION_525)) {
			u8 c1 = 0, c2 = 0, c3 = 0, c4 = 0;
			int mode = 0, cc;

			ivtv_saa7115(itv, DECODER_GET_CC_ODD, &cc);
			if (cc >= 0) {
				mode |= 1;
				c1 = cc & 0xff;
				c2 = cc >> 8;
			}
			ivtv_saa7115(itv, DECODER_GET_CC_EVEN, &cc);
			if (cc >= 0) {
				mode |= 2;
				c3 = cc & 0xff;
				c4 = cc >> 8;
			}
			if (mode) {
				itv->vbi_cc_no_update = 0;
				ivtv_set_cc(itv, mode, c1, c2, c3, c4);
			} else if (itv->vbi_cc_no_update == 4) {
				ivtv_set_cc(itv, 0, 0, 0, 0, 0);
			} else {
				itv->vbi_cc_no_update++;
			}
		}
		return;
	}
	ivtv_set_wss(itv, itv->vbi_wss_found, itv->vbi_wss & 7);
	ivtv_set_cc(itv, itv->vbi_cc_mode, itv->vbi_cc_data[0], itv->vbi_cc_data[1],
			itv->vbi_cc_data[2], itv->vbi_cc_data[3]);
	ivtv_set_vps(itv, itv->vbi_vps_found, itv->vbi_vps[0], itv->vbi_vps[1],
			itv->vbi_vps[2], itv->vbi_vps[3], itv->vbi_vps[4]);
}
