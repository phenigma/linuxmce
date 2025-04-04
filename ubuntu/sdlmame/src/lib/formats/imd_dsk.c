/*********************************************************************

    formats/imd_dsk.c

    IMD disk images

*********************************************************************/

#include <string.h>
#include "flopimg.h"

struct imddsk_tag
{
	int heads;
	int tracks;
	int sector_size;
	UINT64 track_offsets[84*2]; /* offset within data for each track */
};


static struct imddsk_tag *get_tag(floppy_image_legacy *floppy)
{
	struct imddsk_tag *tag;
	tag = (imddsk_tag *)floppy_tag(floppy);
	return tag;
}



FLOPPY_IDENTIFY( imd_dsk_identify )
{
	UINT8 header[3];

	floppy_image_read(floppy, header, 0, 3);
	if (header[0]=='I' && header[1]=='M' && header[2]=='D') {
		*vote = 100;
	} else {
		*vote = 0;
	}
	return FLOPPY_ERROR_SUCCESS;
}

static int imd_get_heads_per_disk(floppy_image_legacy *floppy)
{
	return get_tag(floppy)->heads;
}

static int imd_get_tracks_per_disk(floppy_image_legacy *floppy)
{
	return get_tag(floppy)->tracks;
}

static UINT64 imd_get_track_offset(floppy_image_legacy *floppy, int head, int track)
{
	return get_tag(floppy)->track_offsets[(track<<1) + head];
}

static floperr_t get_offset(floppy_image_legacy *floppy, int head, int track, int sector, int sector_is_index, UINT64 *offset)
{
	UINT64 offs = 0;
	UINT8 header[5];
	UINT8 sector_num;
	int i;


	if ((head < 0) || (head >= get_tag(floppy)->heads) || (track < 0) || (track >= get_tag(floppy)->tracks)
			|| (sector < 0) )
		return FLOPPY_ERROR_SEEKERROR;

	offs = imd_get_track_offset(floppy,head,track);
	floppy_image_read(floppy, header, offs, 5);

	sector_num = header[3];
	offs += 5 + sector_num; // skip header and sector numbering map
	if(header[2] & 0x80) offs += sector_num; // skip cylinder numbering map
	if(header[2] & 0x40) offs += sector_num; // skip head numbering map
	get_tag(floppy)->sector_size = 1 << (header[4] + 7);
	for(i=0;i<sector;i++) {
		floppy_image_read(floppy, header, offs, 1); // take sector data type
		switch(header[0]) {
			case 0: offs++; break;
			case 1:
			case 3:
			case 5:
			case 7: offs += get_tag(floppy)->sector_size + 1; break;
			default: offs += 2;
		}
	}
	if (offset)
		*offset = offs;
	return FLOPPY_ERROR_SUCCESS;
}



static floperr_t internal_imd_read_sector(floppy_image_legacy *floppy, int head, int track, int sector, int sector_is_index, void *buffer, size_t buflen)
{
	UINT64 offset;
	floperr_t err;
	UINT8 header[1];

	// take sector offset
	err = get_offset(floppy, head, track, sector, sector_is_index, &offset);
	if (err)
		return err;

	floppy_image_read(floppy, header, offset, 1);
	switch(header[0]) {
		case 0: break;
		case 1:
		case 3:
		case 5:
		case 7:
				floppy_image_read(floppy, buffer, offset+1, buflen);
				break;

		default: // all data same
				floppy_image_read(floppy, header, offset+1, 1);
				memset(buffer,header[0],buflen);
				break;
	}

	return FLOPPY_ERROR_SUCCESS;
}


static floperr_t imd_read_sector(floppy_image_legacy *floppy, int head, int track, int sector, void *buffer, size_t buflen)
{
	return internal_imd_read_sector(floppy, head, track, sector, FALSE, buffer, buflen);
}

static floperr_t imd_read_indexed_sector(floppy_image_legacy *floppy, int head, int track, int sector, void *buffer, size_t buflen)
{
	return internal_imd_read_sector(floppy, head, track, sector, TRUE, buffer, buflen);
}

static floperr_t imd_get_sector_length(floppy_image_legacy *floppy, int head, int track, int sector, UINT32 *sector_length)
{
	floperr_t err;
	err = get_offset(floppy, head, track, sector, FALSE, NULL);
	if (err)
		return err;

	if (sector_length) {
		*sector_length = get_tag(floppy)->sector_size;
	}
	return FLOPPY_ERROR_SUCCESS;
}

static floperr_t imd_get_indexed_sector_info(floppy_image_legacy *floppy, int head, int track, int sector_index, int *cylinder, int *side, int *sector, UINT32 *sector_length, unsigned long *flags)
{
	UINT64 offset;
	UINT8 header[5];
	UINT8 hd;
	UINT8 tr;
	UINT32 sector_size;
	UINT8 sector_num;

	offset = imd_get_track_offset(floppy,head,track);
	floppy_image_read(floppy, header, offset, 5);
	tr = header[1];
	hd = header[2];
	sector_num = header[3];
	sector_size = 1 << (header[4] + 7);
	if (sector_index >= sector_num) return FLOPPY_ERROR_SEEKERROR;
	if (cylinder) {
		if (head & 0x80) {
			floppy_image_read(floppy, header, offset + 5 + sector_num+ sector_index, 1);
			*cylinder = header[0];
		} else {
			*cylinder = tr;
		}
	}
	if (side) {
		if (head & 0x40) {
			if (head & 0x80) {
				floppy_image_read(floppy, header, offset + 5 + 2 * sector_num+sector_index, 1);
			} else {
				floppy_image_read(floppy, header, offset + 5 + sector_num+sector_index, 1);
			}
			*side = header[0];
		} else {
			*side = hd & 1;
		}
	}
	if (sector) {
		floppy_image_read(floppy, header, offset + 5 + sector_index, 1);
		*sector = header[0];
	}
	if (sector_length) {
		*sector_length = sector_size;
	}
	if (flags) {
		UINT8 skip;
		if (head & 0x40) {
			if (head & 0x80) {
				skip = 3;
			} else {
				skip = 2;
			}
		} else {
			skip = 1;
		}
		floppy_image_read(floppy, header, offset + 5 + skip * sector_num, 1);
		*flags = 0;
		if ((header[0]-1) & 0x02) *flags |= ID_FLAG_DELETED_DATA;
		if ((header[0]-1) & 0x04) *flags |= ID_FLAG_CRC_ERROR_IN_DATA_FIELD;
	}
	return FLOPPY_ERROR_SUCCESS;
}


FLOPPY_CONSTRUCT( imd_dsk_construct )
{
	struct FloppyCallbacks *callbacks;
	struct imddsk_tag *tag;
	UINT8 header[0x100];
	UINT64 pos = 0;
	int sector_size = 0;
	int sector_num;
	int i;
	if(params)
	{
		// create
		return FLOPPY_ERROR_UNSUPPORTED;
	}

	tag = (struct imddsk_tag *) floppy_create_tag(floppy, sizeof(struct imddsk_tag));
	if (!tag)
		return FLOPPY_ERROR_OUTOFMEMORY;

	floppy_image_read(floppy, header, pos, 1);
	while(header[0]!=0x1a) {
		pos++;
		floppy_image_read(floppy, header, pos, 1);
	}
	pos++;
	tag->tracks = 0;
	tag->heads = 1;
	do {
		floppy_image_read(floppy, header, pos, 5);
		if ((header[2] & 1)==1) tag->heads = 2;
		tag->track_offsets[(header[1]<<1) + (header[2] & 1)] = pos;
		sector_num = header[3];
		pos += 5 + sector_num; // skip header and sector numbering map
		if(header[2] & 0x80) pos += sector_num; // skip cylinder numbering map
		if(header[2] & 0x40) pos += sector_num; // skip head numbering map
		sector_size = 1 << (header[4] + 7);
		for(i=0;i<sector_num;i++) {
			floppy_image_read(floppy, header, pos, 1); // take sector data type
			switch(header[0]) {
				case 0: pos++; break;
				case 1:
				case 3:
				case 5:
				case 7: pos += sector_size + 1; break;
				default: pos += 2;break;
			}
		}
		tag->tracks += 1;
	} while(pos < floppy_image_size(floppy));
	if (tag->heads==2) {
		tag->tracks = tag->tracks / 2;
	}
	callbacks = floppy_callbacks(floppy);
	callbacks->read_sector = imd_read_sector;
	callbacks->read_indexed_sector = imd_read_indexed_sector;
	callbacks->get_sector_length = imd_get_sector_length;
	callbacks->get_heads_per_disk = imd_get_heads_per_disk;
	callbacks->get_tracks_per_disk = imd_get_tracks_per_disk;
	callbacks->get_indexed_sector_info = imd_get_indexed_sector_info;

	return FLOPPY_ERROR_SUCCESS;
}


/***************************************************************************

    Copyright Olivier Galibert
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

/*********************************************************************

    formats/imd_dsk.h

    IMD disk images

*********************************************************************/

#include "emu.h"
#include "imd_dsk.h"

imd_format::imd_format()
{
}

const char *imd_format::name() const
{
	return "imd";
}

const char *imd_format::description() const
{
	return "IMD disk image";
}

const char *imd_format::extensions() const
{
	return "imd";
}

void imd_format::fixnum(char *start, char *end) const
{
	end--;
	if(*end != '0')
		return;
	while(end > start) {
		end--;
		if(*end == ' ')
			*end = '0';
		else if(*end != '0')
			return;
	};
}

int imd_format::identify(io_generic *io, UINT32 form_factor)
{
	char h[32];

	io_generic_read(io, h, 0, 31);
	if(h[7] == ':') {
		h[30] = 0;
		for(int i=0; i != 30; i++)
			if(h[i] >= '0' && h[i] <= '9')      \
				h[i] = '0';

		fixnum(h+ 9, h+11);
		fixnum(h+12, h+14);
		fixnum(h+15, h+19);
		fixnum(h+20, h+22);
		fixnum(h+23, h+25);
		fixnum(h+26, h+28);

		if(!strcmp(h, "IMD 0.0: 00/00/0000 00:00:00\015\012"))
			return 100;
	} else {
		h[31] = 0;
		for(int i=0; i != 31; i++)
			if(h[i] >= '0' && h[i] <= '9')      \
				h[i] = '0';

		fixnum(h+10, h+12);
		fixnum(h+13, h+15);
		fixnum(h+16, h+20);
		fixnum(h+21, h+23);
		fixnum(h+24, h+26);
		fixnum(h+27, h+29);

		if(!strcmp(h, "IMD 0.00: 00/00/0000 00:00:00\015\012"))
			return 100;
	}

	return 0;
}

bool imd_format::load(io_generic *io, UINT32 form_factor, floppy_image *image)
{
	int size = io_generic_size(io);
	UINT8 *img = global_alloc_array(UINT8, size);
	io_generic_read(io, img, 0, size);

	int pos;
	for(pos=0; pos < size && img[pos] != 0x1a; pos++);
	pos++;

	if(pos >= size)
		return false;

	while(pos < size) {
		UINT8 mode = img[pos++];
		UINT8 track = img[pos++];
		UINT8 head = img[pos++];
		UINT8 sector_count = img[pos++];
		UINT8 ssize = img[pos++];

		if(ssize == 0xff)
			throw emu_fatalerror("imd_format: Unsupported variable sector size on track %d head %d", track, head);

		UINT32 actual_size = ssize < 7 ? 128 << ssize : 8192;

		static const int rates[3] = { 500000, 300000, 250000 };
		bool fm = mode < 3;
		int rate = rates[mode % 3];
		int rpm = form_factor == floppy_image::FF_8 || (form_factor == floppy_image::FF_525 && rate >= 300000) ? 360 : 300;
		int cell_count = (fm ? 1 : 2)*rate*60/rpm;

		const UINT8 *snum = img+pos;
		pos += sector_count;
		const UINT8 *tnum = head & 0x80 ? img+pos : NULL;
		if(tnum)
			pos += sector_count;
		const UINT8 *hnum = head & 0x40 ? img+pos : NULL;
		if(hnum)
			pos += sector_count;

		head &= 0x3f;

		int gap_3 = calc_default_pc_gap3_size(form_factor, actual_size);

		desc_pc_sector sects[256];

		for(int i=0; i<sector_count; i++) {
			UINT8 stype = img[pos++];
			sects[i].track       = tnum ? tnum[i] : track;
			sects[i].head        = hnum ? hnum[i] : head;
			sects[i].sector      = snum[i];
			sects[i].size        = ssize;
			sects[i].actual_size = actual_size;

			if(stype == 0 || stype > 8) {
				sects[i].data = NULL;

			} else {
				sects[i].deleted = stype == 3 || stype == 4 || stype == 7 || stype == 8;
				sects[i].bad_crc = stype == 5 || stype == 6 || stype == 7 || stype == 8;

				if(stype == 2 || stype == 4 || stype == 6 || stype == 8) {
					sects[i].data = global_alloc_array(UINT8, actual_size);
					memset(sects[i].data, img[pos++], actual_size);

				} else {
					sects[i].data = img + pos;
					pos += actual_size;
				}
			}
		}

		if(fm)
			build_pc_track_fm(track, head, image, cell_count, sector_count, sects, gap_3);
		else
			build_pc_track_mfm(track, head, image, cell_count, sector_count, sects, gap_3);

		for(int i=0; i<sector_count; i++)
			if(sects[i].data && (sects[i].data < img || sects[i].data >= img+size))
				global_free(sects[i].data);
	}

	global_free(img);
	return true;
}


bool imd_format::supports_save() const
{
	return false;
}

const floppy_format_type FLOPPY_IMD_FORMAT = &floppy_image_format_creator<imd_format>;
