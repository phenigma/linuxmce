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

#include "ivtv-driver.h"
#include "ivtv-queue.h"

static int ivtvyuv_prep_user_dma(struct ivtv *itv, struct ivtv_user_dma *dma,
				 struct ivtvyuv_ioctl_dma_host_to_ivtv_args
				 *args)
{
	unsigned long first, last;
	int y_page_count;
	unsigned long uv_sg_size;
	int i, offset;
	unsigned long uaddr;
	unsigned long uaddr_uv;
	int err;
	struct scatterlist *sg;
	unsigned long yBufferOffset;
	unsigned long uvBufferOffset;
	int uv_page_count;
        int decode_height;

	yBufferOffset = IVTV_DEC_MEM_START + yuv_offset[atomic_read(&itv->yuv_info.next_fill_frame)];
	uvBufferOffset = IVTV_DEC_MEM_START + yuv_offset[atomic_read(&itv->yuv_info.next_fill_frame)]+IVTV_YUV_BUFFER_UV_OFFSET;

	decode_height = args->src_h + args->src_y;
	if (decode_height & 31)
		decode_height = (decode_height + 32) & ~31;

	/* Not Allocated */
	if (dma->map == NULL || dma->SGlist == NULL || dma->SGarray == NULL) {
		IVTV_DEBUG_WARN("User DMA not allocated!!!\n");
		return -ENOMEM;
	}

	/* Still in USE */
	if (dma->SG_length || dma->page_count) {
		IVTV_DEBUG_WARN(
			   "prep_user_dma: SG_length %d page_count %d still full?\n",
			   dma->SG_length, dma->page_count);
		return -EBUSY;
	}


	/* Map the y buffer first */
	/* User Input */
	uaddr = ((unsigned long)args->y_source & PAGE_MASK);
	first = (uaddr & PAGE_MASK) >> PAGE_SHIFT;
	last = (((unsigned long)args->y_source + (720 * decode_height) -
		 1) & PAGE_MASK) >> PAGE_SHIFT;

	/* Align to PAGE_SIZE */
	offset = dma->offset = (unsigned long)args->y_source & ~PAGE_MASK;
	dma->page_count = last - first + 1;

	if (dma->page_count <= 0) {
		IVTV_DEBUG_WARN(
			   "prep_user_dma: Error %d page_count\n",
			   dma->page_count);
		return -EINVAL;
	}

	/* Map the uv buffer second */
	/* User Input */
	uaddr_uv = ((unsigned long)args->uv_source & PAGE_MASK);
	first = (uaddr_uv & PAGE_MASK) >> PAGE_SHIFT;
	last =
	    (((unsigned long)args->uv_source + (720 * decode_height / 2) -
	      1) & PAGE_MASK) >> PAGE_SHIFT;

	/* Align to PAGE_SIZE */
	uv_page_count = last - first + 1;

	/* Get user pages for DMA Xfer */
	down_read(&current->mm->mmap_sem);
	err = get_user_pages(current, current->mm,
			     uaddr, dma->page_count, 0, 1, dma->map, NULL);
	if (err == dma->page_count) {
		err = get_user_pages(current, current->mm,
				     uaddr_uv, uv_page_count, 0, 1,
				     &dma->map[dma->page_count], NULL);
	}
	up_read(&current->mm->mmap_sem);

	if (uv_page_count != err) {
		IVTV_DEBUG_WARN(
			   "failed to map user pages, returned %d instead of %d\n",
			   dma->page_count, err);
		return -EINVAL;
	}

	/* Fill SG Array with new values */
	for (i = 0; i < dma->page_count; i++) {
		if (i == dma->page_count - 1) {
			dma->SGlist[i].length =
			    ((unsigned long)args->y_source +
			     (720 * decode_height)) & ~PAGE_MASK;
		} else {
			dma->SGlist[i].length = PAGE_SIZE - offset;
		}
		dma->SGlist[i].offset = offset;
		dma->SGlist[i].page = dma->map[i];
#ifndef LINUX26
		dma->SGlist[i].address = 0;
#endif /* LINUX26 */
		offset = 0;
	}

	offset = dma->offset = (unsigned long)args->uv_source & ~PAGE_MASK;

	/* Fill SG Array with new values */
	for (i = dma->page_count; i < dma->page_count + uv_page_count; i++) {
		if (i == dma->page_count + uv_page_count - 1) {
			dma->SGlist[i].length =
			    ((unsigned long)args->uv_source +
			     (720 * decode_height / 2)) & ~PAGE_MASK;
		} else {
			dma->SGlist[i].length = PAGE_SIZE - offset;
		}
		dma->SGlist[i].offset = offset;
		dma->SGlist[i].page = dma->map[i];
#ifndef LINUX26
		dma->SGlist[i].address = 0;
#endif /* LINUX26 */
		offset = 0;
	}
        y_page_count = dma->page_count;
	dma->page_count += uv_page_count;
	/* Map SG List */
	dma->SG_length =
	    pci_map_sg(itv->dev, dma->SGlist, dma->page_count,
		       PCI_DMA_TODEVICE);

	/* Fill SG Array with new values */
	dma->sg_size = 0;
        uv_sg_size = 0;
	for (i = 0, sg = dma->SGlist; i < dma->SG_length; i++, sg++) {
		dma->SGarray[i].size = sg_dma_len(sg);
		dma->SGarray[i].src = sg_dma_address(sg);
                if (i< y_page_count)
                    dma->SGarray[i].dst = yBufferOffset + dma->sg_size;
                else
                {
                    dma->SGarray[i].dst = uvBufferOffset + uv_sg_size;
                    uv_sg_size += dma->SGarray[i].size;
                }

		dma->sg_size += dma->SGarray[i].size;
	}

	/* Tag SG Array with Interrupt Bit */
	dma->SGarray[dma->SG_length - 1].size |= 0x80000000;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
	pci_dma_sync_single_for_device((struct pci_dev *)itv->dev,
				       dma->SG_handle,
				       (sizeof(struct ivtv_SG_element) *
					IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#else
	pci_dma_sync_single(itv->dev, dma->SG_handle,
			    (sizeof(struct ivtv_SG_element) *
			     IVTV_DMA_SG_OSD_ENT), PCI_DMA_TODEVICE);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

	return 0;             ;
}

static u32 h_filter_table[] = {
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00010000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00020000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00030000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00040000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00050000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00060000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00070000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00080000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x00090000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x000a0000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x000b0000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x000c0000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x000d0000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x000e0000,
	0x00000000, 0x00000000, 0x000001ff, 0x00000000, 0x000f0000,
	0x03ef001a, 0x002d03f7, 0x002d01a6, 0x03ef03f7, 0x0000001a,
	0x03f00017, 0x004203f4, 0x001d01a1, 0x03ee03fa, 0x0001001d,
	0x03f20014, 0x005903f0, 0x000f0198, 0x03ee03fc, 0x00020020,
	0x03f40010, 0x007403ec, 0x0002018d, 0x03ed03fe, 0x00030022,
	0x03f6000b, 0x009203e8, 0x03f8017d, 0x03ee03ff, 0x00040023,
	0x03f80007, 0x00b103e5, 0x03ef0169, 0x03f00000, 0x00050023,
	0x03fa0002, 0x00d303e3, 0x03e90151, 0x03f20000, 0x00060022,
	0x03fc03fd, 0x00f603e1, 0x03e40137, 0x03f50000, 0x00070020,
	0x03fe03f9, 0x011903e2, 0x03e20119, 0x03f903fe, 0x0008001c,
	0x000003f5, 0x013b03e4, 0x03e100fa, 0x03fd03fc, 0x00090018,
	0x000003f1, 0x015d03e8, 0x03e200da, 0x000203fa, 0x000a0012,
	0x000003ef, 0x017a03ee, 0x03e400ba, 0x000703f8, 0x000b000c,
	0x03ff03ed, 0x019503f7, 0x03e7009b, 0x000c03f5, 0x000c0005,
	0x03fe03ec, 0x01aa0002, 0x03eb007c, 0x001103f3, 0x000d03ff,
	0x03fc03ec, 0x01ba0010, 0x03ef0061, 0x001503f1, 0x000e03f8,
	0x03f903ed, 0x01c60020, 0x03f30048, 0x001903ef, 0x000f03f1,
	0x03f30000, 0x00830000, 0x00830115, 0x03f30000, 0x000003ff,
	0x03f20000, 0x008f0004, 0x00780112, 0x03f503fc, 0x00010000,
	0x03f10000, 0x009b0009, 0x006e010f, 0x03f603f9, 0x000203ff,
	0x03f00000, 0x00a7000f, 0x0064010a, 0x03f703f6, 0x000303ff,
	0x03ef0000, 0x00b40016, 0x005a0104, 0x03f803f3, 0x000403fe,
	0x03ee03ff, 0x00c0001d, 0x005100fd, 0x03fa03f1, 0x000503fd,
	0x03ee03ff, 0x00cc0024, 0x004700f4, 0x03fb03f0, 0x000603fd,
	0x03ee03fe, 0x00d7002c, 0x003e00ec, 0x03fc03ef, 0x000703fc,
	0x03ee03fd, 0x00e20035, 0x003500e2, 0x03fd03ee, 0x000803fc,
	0x03ef03fc, 0x00ed003e, 0x002c00d8, 0x03fe03ee, 0x000903fa,
	0x03f003fb, 0x00f70047, 0x002400cd, 0x03ff03ee, 0x000a03f9,
	0x03f103f9, 0x01000052, 0x001d00c2, 0x03ff03ee, 0x000b03f8,
	0x03f303f8, 0x0107005c, 0x001600b6, 0x000003ef, 0x000c03f7,
	0x03f603f7, 0x010d0066, 0x000f00aa, 0x000003f0, 0x000d03f7,
	0x03f903f6, 0x01130070, 0x000a009d, 0x000003f1, 0x000e03f6,
	0x03fc03f4, 0x0118007b, 0x00050091, 0x000003f2, 0x000f03f5,
	0x000003f4, 0x007e0032, 0x007e00b8, 0x00000032, 0x000003f4,
	0x000203f4, 0x00830036, 0x007900b7, 0x03fe002e, 0x000103f5,
	0x000403f4, 0x0088003b, 0x007500b4, 0x03fd002a, 0x000203f5,
	0x000603f4, 0x008d0040, 0x007000b3, 0x03fb0026, 0x000303f5,
	0x000903f4, 0x00920044, 0x006b00b0, 0x03fa0022, 0x000403f6,
	0x000b03f5, 0x00960049, 0x006700ad, 0x03f9001e, 0x000503f6,
	0x000e03f5, 0x009a004e, 0x006200aa, 0x03f8001b, 0x000603f6,
	0x001103f5, 0x009f0053, 0x005d00a6, 0x03f70017, 0x000703f7,
	0x001403f6, 0x00a20058, 0x005800a2, 0x03f60014, 0x000803f8,
	0x001703f7, 0x00a6005c, 0x0053009e, 0x03f60011, 0x000903f8,
	0x001b03f8, 0x00a90061, 0x004d009a, 0x03f5000e, 0x000a03f9,
	0x001e03f9, 0x00ac0066, 0x00490095, 0x03f5000b, 0x000b03f9,
	0x002203fa, 0x00ae006a, 0x00440090, 0x03f50009, 0x000c03fa,
	0x002503fb, 0x00b2006f, 0x003f008c, 0x03f40006, 0x000d03fa,
	0x002903fd, 0x00b30074, 0x003a0086, 0x03f40004, 0x000e03fb,
	0x002d03fe, 0x00b50078, 0x00350081, 0x03f40002, 0x000f03fc,
	0x00180000, 0x0068003e, 0x00680084, 0x0018003e, 0x00000000,
	0x001a0001, 0x006a0041, 0x00650084, 0x0016003c, 0x000103ff,
	0x001c0002, 0x006d0044, 0x00630083, 0x00140039, 0x000203fe,
	0x001f0003, 0x006f0047, 0x00610081, 0x00120037, 0x000303fd,
	0x00210004, 0x00710049, 0x005f0080, 0x00110034, 0x000403fd,
	0x00230006, 0x0073004c, 0x005c007f, 0x000f0032, 0x000503fc,
	0x00260007, 0x0076004f, 0x005a007d, 0x000d002f, 0x000603fb,
	0x00280009, 0x00780051, 0x0057007b, 0x000c002d, 0x000703fb,
	0x002a000a, 0x007a0055, 0x0055007a, 0x000a002a, 0x000803fa,
	0x002d000c, 0x007b0057, 0x00520078, 0x00090028, 0x000903fa,
	0x0030000d, 0x007e005a, 0x004f0076, 0x00070026, 0x000a03f9,
	0x0032000f, 0x007f005d, 0x004d0074, 0x00060023, 0x000b03f9,
	0x00350011, 0x0081005f, 0x004a0072, 0x00050021, 0x000c03f8,
	0x00370013, 0x00830062, 0x00470070, 0x0003001f, 0x000d03f8,
	0x003a0015, 0x00840064, 0x0044006e, 0x0002001d, 0x000e03f8,
	0x003c0016, 0x00850067, 0x0042006c, 0x0001001b, 0x000f03f8
};

static u32 v_filter_table[] = {
	0x01ff0000, 0x00000000, 0x01ff0000, 0x00010000,
	0x01ff0000, 0x00020000, 0x01ff0000, 0x00030000,
	0x01ff0000, 0x00040000, 0x01ff0000, 0x00050000,
	0x01ff0000, 0x00060000, 0x01ff0000, 0x00070000,
	0x01ff0000, 0x00080000, 0x01ff0000, 0x00090000,
	0x01ff0000, 0x000a0000, 0x01ff0000, 0x000b0000,
	0x01ff0000, 0x000c0000, 0x01ff0000, 0x000d0000,
	0x01ff0000, 0x000e0000, 0x01ff0000, 0x000f0000,
	0x01a6002d, 0x0000002d, 0x01a10042, 0x0001001d,
	0x01980059, 0x0002000f, 0x018b0073, 0x00030002,
	0x01780090, 0x000403f8, 0x016300ae, 0x000503ef,
	0x014900ce, 0x000603e9, 0x012d00ee, 0x000703e5,
	0x010f010f, 0x000803e2, 0x00f0012e, 0x000903e2,
	0x00d0014d, 0x000a03e3, 0x00b20169, 0x000b03e5,
	0x00940184, 0x000c03e8, 0x0079019c, 0x000d03eb,
	0x005f01b2, 0x000e03ef, 0x004801c5, 0x000f03f3,
	0x0108007c, 0x0000007c, 0x01050088, 0x00010073,
	0x01030094, 0x00020069, 0x00ff00a1, 0x00030060,
	0x00fb00ae, 0x00040057, 0x00f700bb, 0x0005004e,
	0x00f100c9, 0x00060046, 0x00eb00d7, 0x0007003e,
	0x00e600e6, 0x00080034, 0x00de00f4, 0x0009002e,
	0x00d70103, 0x000a0026, 0x00d00111, 0x000b001f,
	0x00c80120, 0x000c0018, 0x00bf0130, 0x000d0011,
	0x00b6013f, 0x000e000b, 0x00ad014e, 0x000f0005,
	0x00d80094, 0x00000094, 0x00d7009a, 0x0001008f,
	0x00d500a1, 0x0002008a, 0x00d400a7, 0x00030085,
	0x00d200ae, 0x00040080, 0x00d000b5, 0x0005007b,
	0x00ce00bc, 0x00060076, 0x00cc00c2, 0x00070072,
	0x00ca00ca, 0x0008006c, 0x00c700d1, 0x00090068,
	0x00c500d8, 0x000a0063, 0x00c200e0, 0x000b005e,
	0x00bf00e7, 0x000c005a, 0x00bc00ef, 0x000d0055,
	0x00b900f7, 0x000e0050, 0x00b600ff, 0x000f004b,
	0x00c8009c, 0x0000009c, 0x00c700a0, 0x00010099,
	0x00c600a4, 0x00020096, 0x00c400a9, 0x00030093,
	0x00c300ad, 0x00040090, 0x00c200b1, 0x0005008d,
	0x00c100b5, 0x0006008a, 0x00bf00ba, 0x00070087,
	0x00be00be, 0x00080084, 0x00bd00c3, 0x00090080,
	0x00bb00c7, 0x000a007e, 0x00ba00cc, 0x000b007a,
	0x00b900d0, 0x000c0077, 0x00b700d5, 0x000d0074,
	0x00b500da, 0x000e0071, 0x00b400de, 0x000f006e
};

void ivtv_yuv_filter(struct ivtv *itv, int h_filter, int v_filter_1, int v_filter_2)
{
	int filter_index, filter_line;

	// If any filter is -1, then don't update it
	if (h_filter > -1) {
		if (h_filter > 4) h_filter = 4;
		filter_index = h_filter * 80;
		filter_line = 0;
		while (filter_line < 16) {
			writel(h_filter_table[filter_index], itv->reg_mem + 0x02804);
			writel(h_filter_table[filter_index++], itv->reg_mem + 0x0281c);
			writel(h_filter_table[filter_index], itv->reg_mem + 0x02808);
			writel(h_filter_table[filter_index++], itv->reg_mem + 0x02820);
			writel(h_filter_table[filter_index], itv->reg_mem + 0x0280c);
			writel(h_filter_table[filter_index++], itv->reg_mem + 0x02824);
			writel(h_filter_table[filter_index], itv->reg_mem + 0x02810);
			writel(h_filter_table[filter_index++], itv->reg_mem + 0x02828);
			writel(h_filter_table[filter_index], itv->reg_mem + 0x02814);
			writel(h_filter_table[filter_index++], itv->reg_mem + 0x0282c);
			writel(0, itv->reg_mem + 0x02818);
			writel(0, itv->reg_mem + 0x02830);
			filter_line ++;
		}
		IVTV_DEBUG_YUV("h_filter -> %d\n",h_filter);
	}

	if (v_filter_1 > -1) {
		if (v_filter_1 > 4) v_filter_1 = 4;
		filter_index = v_filter_1 * 32;
		filter_line = 0;
		while (filter_line < 16) {
			writel(v_filter_table[filter_index++], itv->reg_mem + 0x02900);
			writel(v_filter_table[filter_index++], itv->reg_mem + 0x02904);
			writel(0, itv->reg_mem + 0x02908);
			filter_line ++;
		}
		IVTV_DEBUG_YUV("v_filter_1 -> %d\n",v_filter_1);
	}

	if (v_filter_2 > -1) {
		if (v_filter_2 > 4) v_filter_2 = 4;
		filter_index = v_filter_2 * 32;
		filter_line = 0;
		while (filter_line < 16) {
			writel(v_filter_table[filter_index++], itv->reg_mem + 0x0290c);
			writel(v_filter_table[filter_index++], itv->reg_mem + 0x02910);
			writel(0, itv->reg_mem + 0x02914);
			filter_line ++;
		}
		IVTV_DEBUG_YUV("v_filter_2 -> %d\n",v_filter_2);
	}
}

// Starting values for calculating reg 2870
static u32 reg_2870_start[] = {
	0x0010000E, 0x000F000C, 0x000E000B, 0x000E000A, 0x000D0009,
	0x000C0008, 0x000C0007, 0x000B0006, 0x000A0005, 0x00080004
};

int ivtv_yuv_prep_frame(struct ivtv *itv,
			       struct ivtvyuv_ioctl_dma_host_to_ivtv_args *args)
{
	int rc = 0;

	int nextFillFrame;
	int lastFillFrame;

	struct ivtv_stream *stream =	/* Use Decoder Stream for locking */
		&itv->streams[IVTV_DEC_STREAM_TYPE_YUV];
        int frame_interlaced;

	nextFillFrame = (atomic_read(&itv->yuv_info.next_fill_frame)+1) & 0x3;
	lastFillFrame = (atomic_read(&itv->yuv_info.next_dma_frame)+1) & 0x3;
        if (atomic_read(&itv->yuv_info.next_fill_frame) != atomic_read(&itv->yuv_info.next_dma_frame) && lastFillFrame != atomic_read(&itv->yuv_info.next_fill_frame))
        {
            return EWOULDBLOCK; 
        }
	// Width for both src & dst must be even. Round up
	args->src_w += args->src_w & 1;
	args->dst_w += args->dst_w & 1;

	args->src_h += args->src_h & 1;
	args->dst_h += args->dst_h & 1;

	// The OSD can be moved. Track to it
	args->dst_x += itv->yuv_info.osd_x_offset;
	args->dst_y += itv->yuv_info.osd_y_offset;

	args->dst_y += args->dst_y & 1;

	// Sorry, but no negative coords for src
	if (args->src_x < 0) args->src_x = 0;
	if (args->src_y < 0) args->src_y = 0;

	// Src coords must also be even. Round down
	args->src_x &= ~1;
	args->src_y &= ~1;

	if ((rc =ivtvyuv_prep_user_dma(itv, &stream->udma, args)) != 0)
		return rc;

	IVTV_OSD_DEBUG_INFO("yuv_prep_frame\n");
	itv->dec_dma_stat.last_xfer = stream->udma.sg_size;
	atomic_set(&itv->dec_dma_stat.type, 1);

	down(&itv->DMA_lock);
	if (dma_to_device(itv, stream, stream->udma.SG_handle, 0) != 0) {
		IVTV_OSD_DEBUG_WARN("Error OSD User DMA\n");
	}
	else {
		atomic_set(&itv->yuv_info.next_fill_frame, nextFillFrame);
	}

	up(&itv->DMA_lock);


        switch (itv->yuv_info.lace_mode) {
        case IVTV_YUV_MODE_PROGRESSIVE: // Progressive mode
        	itv->yuv_info.frame_interlaced = 0;
		if (args->srcBuf_height < 512 || (args->srcBuf_height > 576 && args->srcBuf_height < 1021))
 			frame_interlaced = 0;
                else
 			frame_interlaced = 1;
 		break;
 				
 				
        case IVTV_YUV_MODE_AUTO:
 		if (args->srcBuf_height <= itv->yuv_info.lace_threshold || args->srcBuf_height > 576 || args->srcBuf_width > 720){
 			itv->yuv_info.frame_interlaced = 0;
			if ((args->srcBuf_height < 512) || (args->srcBuf_height > 576 && args->srcBuf_height < 1021) || (args->srcBuf_width > 720 && args->srcBuf_height < 1021))
 				frame_interlaced = 0;
 			else
 				frame_interlaced = 1;
 		}
 		else {
 			itv->yuv_info.frame_interlaced = 1;
 			frame_interlaced = 1;
 		}
 		break;
        case IVTV_YUV_MODE_INTERLACED: // Interlace mode
        default:
 		itv->yuv_info.frame_interlaced = 1;
 		frame_interlaced = 1;
 		break;
  	}

	// dst_x must be even only if it's negative
	if (args->dst_x < 0) args->dst_x &= ~1;

	// Can only reduce width down to 1/4 original size
	if (args->src_w / 4 > args->dst_w) {
//		IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, "Horizontal scaling limit exceeded.\n");
		args->dst_w = args->src_w / 4;
	}
		
	// Can only reduce height down to 1/4 original size
	if (args->src_h / args->dst_h >= 2) {
		// Overflow may be because we're running progressive, so force mode switch
		frame_interlaced = 1;
		// Make sure we're still within limits for interlace
		if (args->src_h / args->dst_h >= 4) {
			// If we reach here we'll have to force the height.
//			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, "Vertical scaling limit exceeded. Requested %d\n", newHeight);
			args->dst_h = args->src_h / 4;
		}
	}

	// Both x offset & width are linked, so they have to be done together
	if ((itv->yuv_info.yuv_dst_w != args->dst_w) ||
	    (itv->yuv_info.yuv_src_w != args->src_w) ||
	    (itv->yuv_info.yuv_dst_x != args->dst_x) ||
	    (itv->yuv_info.yuv_src_x != args->src_x)) {
		u32 reg_2834, reg_2838, reg_283c;
		u32 reg_2844, reg_2854, reg_285c;
		u32 reg_2864, reg_2874, reg_2890;
		u32 reg_2870, reg_2870_base, reg_2870_offset;
		int tmp_x, tmp_w, x_cutoff;
		int x_offset_minor, x_offset_major, x_offset_calc;
		int h_filter;
		u32 master_width;

		IVTV_OSD_DEBUG_WARN( "Need to adjust to width %d src_w %d dst_w %d src_x %d dst_x %d\n",
				     args->srcBuf_width, args->src_w, args->dst_w,args->src_x, args->dst_x);
		if (itv->yuv_info.yuv_src_w == 0) {
			itv->yuv_info.reg_2834 = readl( itv->reg_mem + 0x02834);
			itv->yuv_info.reg_2838 = readl( itv->reg_mem + 0x02838);
			itv->yuv_info.reg_283c = readl( itv->reg_mem + 0x0283c);
			itv->yuv_info.reg_2840 = readl( itv->reg_mem + 0x02840);
			itv->yuv_info.reg_2844 = readl( itv->reg_mem + 0x02844);
			itv->yuv_info.reg_2848 = readl( itv->reg_mem + 0x02848);
			itv->yuv_info.reg_2854 = readl( itv->reg_mem + 0x02854);
			itv->yuv_info.reg_285c = readl( itv->reg_mem + 0x0285c);
			itv->yuv_info.reg_2864 = readl( itv->reg_mem + 0x02864);
			itv->yuv_info.reg_2870 = readl( itv->reg_mem + 0x02870);
			itv->yuv_info.reg_2874 = readl( itv->reg_mem + 0x02874);
			itv->yuv_info.reg_2890 = readl( itv->reg_mem + 0x02890);
		}

		tmp_x = args->dst_x;
		tmp_w = args->dst_w;

		// Handle negative dst_x
		x_offset_minor = 0;
		x_offset_major = 0;

		// We need to manipulate 2 registers for the best range
		if (tmp_x < 0){
			// How wide is 1 pixel source after scaling
			x_offset_calc = (tmp_w << 16)/args->src_w;
			x_offset_major = ((-tmp_x) <<16) / x_offset_calc;
			tmp_x += x_offset_major;
		}
		else x_offset_minor = tmp_x;

		// How wide is the src image
		x_cutoff  = args->src_w + args->src_x;

		// If we overflow the right of the screen, we must clip the image
		if (tmp_w + tmp_x > 720) {
			x_cutoff = args->src_x+(((720 - args->dst_x) * ((args->src_w << 16) / args->dst_w))>>16);
			x_cutoff &= ~1; 
			tmp_w -= (args->dst_w + tmp_x) - 720;
		}

		if (tmp_w > 720) tmp_w = 720;

		// Just in case...
		if (x_cutoff > 720) {
			IVTV_DEBUG_YUV("OOPS! x_cutoff overflow : %d\n",x_cutoff);
			x_cutoff = 720;
		}

		// Set the display width
		reg_2834 = tmp_w;
		// If we're off the left, clip a bit more.
		if ((args->dst_x < 0) && (args->dst_w <= 720)) reg_2834 += args->dst_x;

		if (reg_2834 > 720) {
			IVTV_DEBUG_YUV("OOPS! reg 2834 overflow : %d\n",reg_2834);
			reg_2834 = 720-args->dst_x;
		}
		reg_2838 = reg_2834;

		// The default setting for 2870;
		reg_2870 = reg_2870_start[0];
			
		// 2870 is fudged for running video in a window
		// If running full screen, it causes an unwanted left shift
		// Remove the fudge if we almost fill the screen.
		// Gradually adjust the offset to avoid the video 'snapping'
		// left/right if it gets dragged through this region.
		if ((args->dst_x > -1) && (args->dst_x <=36) && ((args->dst_x + args->dst_w >= 684))){
			reg_2870 = reg_2870_start[(36 - args->dst_x)/4];
//			reg_2870 = reg_2870 - (((42 - args->dst_x) /6) << 16) - ((36 - args->dst_x) / 4);
		}
		else if ((args->dst_x < 0) && (args->dst_x >= -18) && (args->dst_w >=702)) {
			reg_2870 = reg_2870_start[(18 + args->dst_x)/2];
//			reg_2870 = reg_2870 - (((21 + args->dst_x) /3) << 16) - ((18 + args->dst_x) / 2);
		}

		// We're also using 2870 to shift the image left (src_x & negative dst_x)
		reg_2870_offset = (args->src_x*((args->dst_w << 16)/args->src_w))>>16;
		if (args->dst_x < 0) reg_2870_offset -= args->dst_x;
			
		if (args->dst_w >= args->src_w) {
			x_cutoff &= ~1;
			if (args->dst_w > 720) x_cutoff -= 2;
			master_width = (args->src_w * 0x00200000) / (args->dst_w);
			if (master_width * args->dst_w != args->src_w * 0x00200000) master_width ++;
			reg_2834 = (reg_2834 << 16) | x_cutoff;
			reg_2838 = (reg_2838 << 16) | x_cutoff;
			reg_283c = master_width >> 2;
			reg_2844 = master_width >> 2;
			reg_2854 = master_width;
			reg_285c = master_width >> 1;
			reg_2864 = master_width >> 1;

			// We also need to factor in the scaling
			// (src_w - dst_w) / (src_w / 4)
			if (args->dst_w > args->src_w)
				reg_2870_base = ((args->dst_w - args->src_w)<<16) / (args->src_w <<14);
			else
				reg_2870_base = 0;

			reg_2870 = reg_2870 + (reg_2870_offset << 16 | reg_2870_offset) + (reg_2870_base << 17 | reg_2870_base);
			reg_2874 = 0;
		}
		else if (args->dst_w < args->src_w / 2) {
			master_width = (args->src_w * 0x00080000) / args->dst_w;
			if (master_width * args->dst_w != args->src_w * 0x00080000) master_width ++;
			reg_2834 = (reg_2834 << 16) | x_cutoff;
			reg_2838 = (reg_2838 << 16) | x_cutoff;
			reg_283c = master_width >> 2;
			reg_2844 = master_width >> 1;
			reg_2854 = master_width;
			reg_285c = master_width >> 1;
			reg_2864 = master_width >> 1;
			reg_2870 = reg_2870 + (reg_2870_offset << 17 | reg_2870_offset << 2);
			reg_2874 = 0x00000012;
		}
		else {
			master_width = (args->src_w * 0x00100000) / args->dst_w;
			if (master_width * args->dst_w != args->src_w * 0x00100000) master_width ++;
			reg_2834 = (reg_2834 << 16) | x_cutoff;
			reg_2838 = (reg_2838 << 16) | x_cutoff;
			reg_283c = master_width >> 2;
			reg_2844 = master_width >> 1;
			reg_2854 = master_width;
			reg_285c = master_width >> 1;
			reg_2864 = master_width >> 1;
			reg_2870 = reg_2870 + (reg_2870_offset << 16 | reg_2870_offset << 1);
			reg_2874 = 0x00000001;
		}

		// Select the horizontal filter
		if (args->src_w == args->dst_w) {
			// An exact size match uses filter 0
			h_filter = 0;
		}
		else {
			// Figure out which filter to use
			h_filter = ((args->src_w << 16) / args->dst_w) >> 15;
			h_filter = (h_filter >> 1) + (h_filter & 1);
			// Only an exact size match can use filter 0.
			if (h_filter == 0) h_filter = 1;
		}

		writel(reg_2834, itv->reg_mem + 0x02834);
		writel(reg_2838, itv->reg_mem + 0x02838);
		IVTV_DEBUG_YUV("Update reg 0x2834 %08x->%08x 0x2838 %08x->%08x\n",itv->yuv_info.reg_2834, reg_2834, itv->yuv_info.reg_2838, reg_2838);

		writel(reg_283c, itv->reg_mem + 0x0283c);
		writel(reg_2844, itv->reg_mem + 0x02844);

		IVTV_DEBUG_YUV("Update reg 0x283c %08x->%08x 0x2844 %08x->%08x\n",itv->yuv_info.reg_283c, reg_283c, itv->yuv_info.reg_2844, reg_2844);

		writel(0x00080514, itv->reg_mem + 0x02840);
		writel(0x00100514, itv->reg_mem + 0x02848);
		IVTV_DEBUG_YUV("Update reg 0x2840 %08x->%08x 0x2848 %08x->%08x\n",itv->yuv_info.reg_2840, 0x00080514, itv->yuv_info.reg_2848, 0x00100514);
			
		writel(reg_2854, itv->reg_mem + 0x02854);
		IVTV_DEBUG_YUV("Update reg 0x2854 %08x->%08x \n",itv->yuv_info.reg_2854, reg_2854);

		writel(reg_285c, itv->reg_mem + 0x0285c);
		writel(reg_2864, itv->reg_mem + 0x02864);
		IVTV_DEBUG_YUV("Update reg 0x285c %08x->%08x 0x2864 %08x->%08x\n",itv->yuv_info.reg_285c, reg_285c, itv->yuv_info.reg_2864, reg_2864);

		// Set the horizontal divider
		writel(reg_2874, itv->reg_mem + 0x02874);
		IVTV_DEBUG_YUV("Update reg 0x2874 %08x->%08x\n",itv->yuv_info.reg_2874, reg_2874);

		// Set the src offset
		writel(reg_2870, itv->reg_mem + 0x02870);
		IVTV_DEBUG_YUV("Update reg 0x2870 %08x->%08x\n",itv->yuv_info.reg_2870, reg_2870);

		reg_2890 = x_offset_minor;
		writel( reg_2890,itv->reg_mem+0x02890);
		IVTV_DEBUG_YUV("Update reg 0x2890 %08x->%08x\n",itv->yuv_info.reg_2890, reg_2890);

		// Only update the filter if we really need to
		if (h_filter != itv->yuv_info.h_filter) {
			ivtv_yuv_filter (itv,h_filter,-1,-1);
			itv->yuv_info.h_filter = h_filter;
		}

		itv->yuv_info.yuv_dst_w = args->dst_w;
		itv->yuv_info.yuv_src_w = args->src_w;
		itv->yuv_info.yuv_dst_x = args->dst_x;
		itv->yuv_info.yuv_src_x = args->src_x;
	}

	if ((itv->yuv_info.yuv_src_h != args->src_h) ||
	    (itv->yuv_info.yuv_dst_h != args->dst_h) ||
	    (itv->yuv_info.yuv_dst_y != args->dst_y) ||
	    (itv->yuv_info.yuv_src_y != args->src_y) ||
	    (itv->yuv_info.frame_interlaced != itv->yuv_info.frame_interlaced_last) ||
	    (itv->yuv_info.lace_threshold != itv->yuv_info.lace_threshold_last)) {
		u32 master_height;
		u32 reg_2918, reg_291c, reg_2920, reg_2928;
		u32 reg_2930, reg_2934, reg_293c;
		u32 reg_2940, reg_2944, reg_294c;
		u32 reg_2950, reg_2954, reg_2958, reg_295c;
		u32 reg_2960, reg_2964, reg_2968, reg_296c;
		u32 reg_289c;
		int y_offset_minor, y_offset_major, y_offset_calc;
		int y_cutoff, tmp_y, tmp_h;
		int v_filter_1, v_filter_2;

		IVTV_OSD_DEBUG_WARN("Need to adjust to height %d src_h %d dst_h %d src_y %d dst_y %d\n",
				    args->srcBuf_height, args->src_h, args->dst_h,args->src_y, args->dst_y);

		// What scaling mode is being used...
		if (frame_interlaced) {
			IVTV_DEBUG_YUV("Scaling mode: Interlaced\n");
		}
		else {
			IVTV_DEBUG_YUV("Scaling mode: Progressive\n");
		}

		// What is the source video being treated as...
		if (itv->yuv_info.frame_interlaced) {
			IVTV_OSD_DEBUG_WARN("Source video: Interlaced\n");
		}
		else {
			IVTV_OSD_DEBUG_WARN("Source video: Non-interlaced\n");
		}

		if (itv->yuv_info.yuv_src_h == 0) {
			itv->yuv_info.reg_289c = readl( itv->reg_mem + 0x0289c);
			itv->yuv_info.reg_2918 = readl( itv->reg_mem + 0x02918);
			itv->yuv_info.reg_291c = readl( itv->reg_mem + 0x0291c);
			itv->yuv_info.reg_2920 = readl( itv->reg_mem + 0x02920);
			itv->yuv_info.reg_2924 = readl( itv->reg_mem + 0x02924);
			itv->yuv_info.reg_2928 = readl( itv->reg_mem + 0x02928);
			itv->yuv_info.reg_292c = readl( itv->reg_mem + 0x0292c);
			itv->yuv_info.reg_2930 = readl( itv->reg_mem + 0x02930);
			itv->yuv_info.reg_2934 = readl( itv->reg_mem + 0x02934);
			itv->yuv_info.reg_2938 = readl( itv->reg_mem + 0x02938);
			itv->yuv_info.reg_293c = readl( itv->reg_mem + 0x0293c);
			itv->yuv_info.reg_2940 = readl( itv->reg_mem + 0x02940);
			itv->yuv_info.reg_2944 = readl( itv->reg_mem + 0x02944);
			itv->yuv_info.reg_2948 = readl( itv->reg_mem + 0x02948);
			itv->yuv_info.reg_294c = readl( itv->reg_mem + 0x0294c);

			itv->yuv_info.reg_2950 = readl( itv->reg_mem + 0x02950);
			itv->yuv_info.reg_2954 = readl( itv->reg_mem + 0x02954);
			itv->yuv_info.reg_2958 = readl( itv->reg_mem + 0x02958);
			itv->yuv_info.reg_295c = readl( itv->reg_mem + 0x0295c);
			itv->yuv_info.reg_2960 = readl( itv->reg_mem + 0x02960);
			itv->yuv_info.reg_2964 = readl( itv->reg_mem + 0x02964);
			itv->yuv_info.reg_2968 = readl( itv->reg_mem + 0x02968);
			itv->yuv_info.reg_296c = readl( itv->reg_mem + 0x0296c);
			itv->yuv_info.reg_2970 = readl( itv->reg_mem + 0x02970);
		}

		tmp_y = args->dst_y;
		tmp_h = args->dst_h;

		// Handle negative dst_y
		y_offset_minor = 0;
		y_offset_major = 0;

		// We need to manipulate 2 registers for the best range
		if (tmp_y < 0){
			// How tall is 1 pixel source after scaling
			y_offset_calc = (tmp_h << 16)/args->src_h;
			y_offset_major = ((-tmp_y) << 16) / y_offset_calc;
			y_offset_minor = -tmp_y - ((y_offset_major * y_offset_calc)>>16);
			tmp_y += y_offset_major;
		}
		else
			y_offset_minor = tmp_y;

		// Where is the bottom line (Use true src height here)
		y_cutoff = args->dst_h + args->src_y + y_offset_minor + 1;
		if (args->dst_y > 0)
			y_cutoff = tmp_h + (tmp_h < itv->yuv_info.decode_height);

		// Clip the image at the bottom of the screen
		if (y_cutoff + tmp_y > itv->yuv_info.decode_height)
			y_cutoff = itv->yuv_info.decode_height - tmp_y;

		if (y_cutoff > itv->yuv_info.decode_height) {
			IVTV_DEBUG_YUV("OOPS! y_cutoff overflow : %d\n",y_cutoff);
			y_cutoff = itv->yuv_info.decode_height;
		}

		// Use src height here
		if (frame_interlaced) {
			reg_2918 = (y_cutoff << 16) | args->src_h;
			reg_291c = (y_cutoff << 16) | (args->src_h - 1);
		}
		else {
			reg_2918 = (y_cutoff << 16) | (args->src_h << 1);
			reg_291c = (y_cutoff << 16) | args->src_h;
		}

		if (args->dst_h / 2 >= args->src_h && !frame_interlaced) {
//			IVTV_DEBUG_YUV("MODE 1\n");
			master_height = (args->src_h * 0x00400000) / args->dst_h;
			if ((args->src_h * 0x00400000) - (master_height * args->dst_h) >= args->dst_h / 2) master_height ++;
			reg_2920 = master_height >> 2;
			reg_2928 = master_height >> 3;
			reg_2930 = master_height;
			reg_2940 = master_height >> 1;
			reg_296c = 0x00000000;
		}
		else if (args->dst_h >= args->src_h) {	
//			IVTV_DEBUG_YUV("MODE 2\n");
			master_height = (args->src_h * 0x00400000) / args->dst_h;
			master_height = (master_height >> 1) + (master_height & 1);
			reg_2920 = master_height >> 2;
			reg_2928 = master_height >> 2;
			reg_2930 = master_height;
			reg_2940 = master_height >> 1;
			reg_296c = 0x00000000;
			if (!frame_interlaced) reg_296c ++;
		}
		else if (args->dst_h >= args->src_h / 2) {
//			IVTV_DEBUG_YUV("MODE 3\n");
			master_height = (args->src_h * 0x00200000) / args->dst_h;
			master_height = (master_height >> 1) + (master_height & 1);
			reg_2920 = master_height >> 2;
			reg_2928 = master_height >> 2;
			reg_2930 = master_height;
			reg_2940 = master_height;
			reg_296c = 0x00000101;
			if (!frame_interlaced) reg_296c ++;
		}
		else {
//			IVTV_DEBUG_YUV("MODE 4\n");
			master_height = (args->src_h * 0x00100000) / args->dst_h;
			master_height = (master_height >> 1) + (master_height & 1);
			reg_2920 = master_height >> 2;
			reg_2928 = master_height >> 2;
			reg_2930 = master_height;
			reg_2940 = master_height;
			reg_296c = 0x00000102;
		}

		// FIXME These registers change depending on scaled / unscaled output
		// We really need to work out what they should be
			
		if (args->src_h == args->dst_h){
			reg_2934 = 0x00020000;
			reg_293c = 0x00100000;
			reg_2944 = 0x00040000;
			reg_294c = 0x000b0000;
		}
		else {
			reg_2934 = 0x00000FF0;
			reg_293c = 0x00000FF0;
			reg_2944 = 0x00000FF0;
			reg_294c = 0x00000FF0;
		}

		// The first line to be displayed
		reg_2950 = 0x00010000 + y_offset_major + args->src_y;
		reg_2958 = 0x00010000 + ((y_offset_major + args->src_y) >> 1);
		reg_2954 = 0x00010001 + y_offset_major + args->src_y;
		reg_295c = 0x00010001 + ((y_offset_major + args->src_y) >> 1);

		if (frame_interlaced) {
			reg_2950 += 0x00010000;
			reg_2954 += 0x00010000;
		}

		if (args->dst_h < args->src_h / 2) {
			reg_2958 += 0x00010000;
			reg_295c += 0x00010000;
		}

		if (itv->yuv_info.decode_height == 480)
			reg_289c = 0x011e0017;
		else
			reg_289c = 0x01500017;

		if (args->dst_y < 0)
			reg_289c = (reg_289c - ((y_offset_minor & ~1)<<15))-(y_offset_minor >>1);
		else
			reg_289c = (reg_289c + ((y_offset_minor & ~1)<<15))+(y_offset_minor >>1);

		// How much of the source to decode.
		// Take into account the source offset
		reg_2960 = (args->src_h + args->src_y) - 1;
		reg_2960 |= (reg_2960 & ~1) << 15;

		// Calculate correct value for register 2964
		if (args->src_h == args->dst_h)
			reg_2964 = 1;
		else {
			reg_2964 = 2 + ((args->dst_h << 1) / args->src_h);
			reg_2964 = (reg_2964 >> 1) + (reg_2964 & 1);
		}
		reg_2968 = (reg_2964 << 16) + reg_2964 + (reg_2964 >> 1);
		reg_2964 = (reg_2964 << 16) + reg_2964 + (reg_2964 * 46 / 94);

		// Okay, we've wasted time working out the correct value,
		// but if we use it, it fouls the the window alignment.
		// Fudge it to what we want...
		reg_2964 = 0x00010001 + ((reg_2964 & 0x0000FFFF) - (reg_2964 >> 16));
		reg_2968 = 0x00010001 + ((reg_2968 & 0x0000FFFF) - (reg_2968 >> 16));

		// Deviate further from what it should be. I find the flicker headache
		// inducing so try to reduce it slightly. Leave 2968 as-is otherwise
		// colours foul.
		if ((reg_2964 != 0x00010001) && (args->dst_h / 2 <= args->src_h))
			reg_2964 = (reg_2964 & 0xFFFF0000) + ((reg_2964 & 0x0000FFFF)/2);

		if (args->dst_h / 2 <= args->src_h) reg_2968 -= 0x00010001;

		// Select the vertical filter
		if (args->src_h == args->dst_h) {
			// An exact size match uses filter 0/1
			v_filter_1 = 0;
			v_filter_2 = 1;
		}
		else {
			// Figure out which filter to use
			v_filter_1 = ((args->src_h << 16) / args->dst_h) >> 15;
			v_filter_1 = (v_filter_1 >> 1) + (v_filter_1 & 1);
			// Only an exact size match can use filter 0.
			if (v_filter_1 == 0) v_filter_1 = 1;
			v_filter_2 = v_filter_1;
		}

		writel(reg_2934, itv->reg_mem + 0x02934);
		writel(reg_293c, itv->reg_mem + 0x0293c);
		IVTV_DEBUG_YUV("Update reg 0x2934 %08x->%08x 0x293c %08x->%08x\n",itv->yuv_info.reg_2934, reg_2934, itv->yuv_info.reg_293c, reg_293c);
		writel(reg_2944, itv->reg_mem + 0x02944);
		writel(reg_294c, itv->reg_mem + 0x0294c);
		IVTV_DEBUG_YUV("Update reg 0x2944 %08x->%08x 0x294c %08x->%08x\n",itv->yuv_info.reg_2944, reg_2944, itv->yuv_info.reg_294c, reg_294c);

		// Ensure 2970 is 0 (does it ever change ?)
//		writel(0,itv->reg_mem+0x02970);
//		IVTV_DEBUG_YUV("Update reg 0x2970 %08x->%08x\n",itv->yuv_info.reg_2970, 0);

		writel(reg_2930, itv->reg_mem + 0x02938);
		writel(reg_2930, itv->reg_mem + 0x02930);
		IVTV_DEBUG_YUV("Update reg 0x2930 %08x->%08x 0x2938 %08x->%08x\n",itv->yuv_info.reg_2930, reg_2930, itv->yuv_info.reg_2938, reg_2930);
		
		writel(reg_2928, itv->reg_mem + 0x02928);
		writel(reg_2928+0x514, itv->reg_mem + 0x0292C);
		IVTV_DEBUG_YUV("Update reg 0x2928 %08x->%08x 0x292c %08x->%08x\n",itv->yuv_info.reg_2928, reg_2928, itv->yuv_info.reg_292c, reg_2928+0x514);

		writel(reg_2920, itv->reg_mem + 0x02920);
		writel(reg_2920+0x514, itv->reg_mem + 0x02924);
		IVTV_DEBUG_YUV("Update reg 0x2920 %08x->%08x 0x2924 %08x->%08x\n",itv->yuv_info.reg_2920, reg_2920, itv->yuv_info.reg_2924, 0x514+reg_2920);

		writel (reg_2918,itv->reg_mem + 0x02918);
		writel (reg_291c,itv->reg_mem + 0x0291C);
		IVTV_DEBUG_YUV("Update reg 0x2918 %08x->%08x 0x291C %08x->%08x\n",itv->yuv_info.reg_2918,reg_2918,itv->yuv_info.reg_291c,reg_291c);

		writel(reg_296c, itv->reg_mem + 0x0296c);
		IVTV_DEBUG_YUV("Update reg 0x296c %08x->%08x\n",itv->yuv_info.reg_296c, reg_296c);

		writel(reg_2940, itv->reg_mem + 0x02948);
		writel(reg_2940, itv->reg_mem + 0x02940);
		IVTV_DEBUG_YUV("Update reg 0x2940 %08x->%08x 0x2948 %08x->%08x\n",itv->yuv_info.reg_2940, reg_2940, itv->yuv_info.reg_2948, reg_2940);

		writel(reg_2950, itv->reg_mem + 0x02950);
		writel(reg_2954, itv->reg_mem + 0x02954);
		IVTV_DEBUG_YUV("Update reg 0x2950 %08x->%08x 0x2954 %08x->%08x\n",itv->yuv_info.reg_2950, reg_2950, itv->yuv_info.reg_2954, reg_2954);

		writel(reg_2958, itv->reg_mem + 0x02958);
		writel(reg_295c, itv->reg_mem + 0x0295C);
		IVTV_DEBUG_YUV("Update reg 0x2958 %08x->%08x 0x295C %08x->%08x\n",itv->yuv_info.reg_2958, reg_2958, itv->yuv_info.reg_295c, reg_295c); 

		writel(reg_2960, itv->reg_mem + 0x02960);
		IVTV_DEBUG_YUV("Update reg 0x2960 %08x->%08x \n",itv->yuv_info.reg_2960, reg_2960);

		writel(reg_2964, itv->reg_mem + 0x02964);
		writel(reg_2968, itv->reg_mem + 0x02968);
		IVTV_DEBUG_YUV("Update reg 0x2964 %08x->%08x 0x2968 %08x->%08x\n",itv->yuv_info.reg_2964, reg_2964, itv->yuv_info.reg_2968, reg_2968);

		writel( reg_289c,itv->reg_mem+0x0289c);
		IVTV_DEBUG_YUV("Update reg 0x289c %08x->%08x\n",itv->yuv_info.reg_289c, reg_289c);

		// Only update filter 1 if we really need to
		if (v_filter_1 != itv->yuv_info.v_filter_1) {
			ivtv_yuv_filter (itv,-1,v_filter_1,-1);
			itv->yuv_info.v_filter_1 = v_filter_1;
		}
		
		// Only update filter 2 if we really need to
		if (v_filter_2 != itv->yuv_info.v_filter_2) {
			ivtv_yuv_filter (itv,-1,-1,v_filter_2);
			itv->yuv_info.v_filter_2 = v_filter_2;
		}

		itv->yuv_info.yuv_src_h = args->src_h;
		itv->yuv_info.yuv_dst_h = args->dst_h;
		itv->yuv_info.yuv_dst_y = args->dst_y;
		itv->yuv_info.yuv_src_y = args->src_y;
		itv->yuv_info.frame_interlaced_last = itv->yuv_info.frame_interlaced;
		itv->yuv_info.lace_threshold_last = itv->yuv_info.lace_threshold;
	}
	/* Unmap Last DMA Xfer */
	if (ivtv_unmap_user_dma(itv, &stream->udma) == 1) {
		IVTV_OSD_DEBUG_WARN( "Error: unmap osd dma\n");
	}
	return rc;
}
