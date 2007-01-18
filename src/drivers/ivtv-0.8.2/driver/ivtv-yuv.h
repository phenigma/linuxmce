/*
    yuv support

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

int ivtv_yuv_prep_frame(struct ivtv *itv,
			       struct ivtvyuv_ioctl_dma_host_to_ivtv_args *args);
void ivtv_yuv_filter(struct ivtv *itv, int h_filter, int v_filter_1, int v_filter_2);
