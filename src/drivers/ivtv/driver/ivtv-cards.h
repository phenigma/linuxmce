/*
    Functions to query card hardware
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>

    Audio input/output:
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

struct ivtv_card_pci_info {
	u16 device;
	u16 subsystem_vendor;
	u16 subsystem_device;
};

/* for card information/parameters */
struct ivtv_card {
	int 		type;
	char           *name;
	u32            	v4l2_capabilities;
	u32 	       	i2c_tuner_addr;
	u32 	       	audio_selector;
	/* list of device and subsystem vendor/devices that
	   correspond to this card type. */
	const struct ivtv_card_pci_info *pci_list;
};

const struct v4l2_input *ivtv_get_input(struct ivtv *itv, u16 index);
const struct v4l2_output *ivtv_get_output(struct ivtv *itv, u16 index);
const struct v4l2_audio *ivtv_get_audio_input(struct ivtv *itv, u16 index);
const struct v4l2_audioout *ivtv_get_audio_output(struct ivtv *itv, u16 index);
const struct ivtv_card *ivtv_get_card(u16 index);
