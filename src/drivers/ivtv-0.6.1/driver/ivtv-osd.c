/*
    On Screen Display cx23415 Framebuffer driver
 
    This module presents the cx23415 OSD (onscreen display) framebuffer memory 
    as a standard Linux /dev/fb style framebuffer device. The framebuffer has
    a 32 bpp packed pixel format with full alpha channel support. Depending
    on the TV standard configured in the ivtv module at load time, resolution
    is fixed at either 720x480 (NTSC) or 720x576 (PAL).

    Copyright (c) 2003 Matt T. Yourst <yourst@yourst.com>
  
    Derived from drivers/video/vesafb.c
    Portions (c) 1998 Gerd Knorr <kraxel@goldbach.in-berlin.de>

    2.6 kernel port:
    Copyright (C) 2004 Matthias Badaire

    Copyright (C) 2004  Chris Kennedy <c@groovy.org>

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

/*
#
# Instructions for making ivtv-fb work with XFree86:
# Add the following sections and parts thereof to /etc/X11/XF86Config:
#

#
# NOTE: The monitor section is obtainable by running:
# fbset -fb /dev/fb1 -x
# (or /dev/fbX for whatever framebuffer ivtv-fb is on)
#
Section "Monitor"
    Identifier  "NTSC Monitor"
    HorizSync  30-68
    VertRefresh 50-120
    Mode "720x480"
      # D: 34.563 MHz, H: 37.244 kHz, V: 73.897 Hz
      DotClock 34.564
      HTimings 720 752 840 928
      VTimings 480 484 488 504
      Flags    "-HSync" "-VSync"
    EndMode
EndSection
# Uncomment the following for PAL
#Section "Monitor"
#    Identifier  "PAL Monitor"
#    HorizSync  30-68
#    VertRefresh 50-120
#    Mode "720x576"
#    # D: 41.475 MHz, H: 44.693 kHz, V: 74.488 Hz
#      DotClock 41.476
#      HTimings 720 752 840 928
#      VTimings 576 580 584 600
#      Flags    "-HSync" "-VSync"
#    EndMode
#EndSection

Section "Device"
    Identifier  "Hauppauge PVR 350 cx23415 Framebuffer"
    Driver      "fbdev"
    Option      "fbdev" "/dev/fb1"      # <-- modify if using another device
    BusID "0:10:0"
EndSection

Section "Screen"
  Identifier  "TV Screen"
  Device      "Hauppauge PVR 350 cx23415 Framebuffer"
  Monitor     "NTSC Monitor"            # <-- select for NTSC
#  Monitor     "PAL Monitor"            # <-- select for PAL
  DefaultDepth 24
  DefaultFbbpp 32
  Subsection "Display"
    Depth 24
    FbBpp 32
    Modes "720x480"                     # <-- select for NTSC
#    Modes "720x576"                    # <-- select for PAL
  EndSubsection
EndSection

Section "ServerLayout"
  ...

  Screen 0 "Screen 1"                      # << (your computer monitor)

  # (add the following line)
  Screen 1 "TV Screen" RightOf "Screen 1"  # << (TV screen)

  ...
EndSection

#
# Then start X as usual; both your normal (computer) monitor and the
# NTSC or PAL TV monitor should display the default X background.
#
# Note the "RightOf" clause above: if you move the mouse off the right
# side of the computer screen, the pointer should appear on your TV
# screen. Keyboard events will go to windows in either screen.
#
# To start a program (e.g., xterm) on the TV only:
#
# export DISPLAY=:0.1         (i.e., X server #0, screen #1 = TV)
# xterm&
#
# There is also a way to join both the computer monitor and TV into
# one giant virtual screen using the Xinerama extension, but I haven't 
# tried it. Doing so may not be such a good idea anyway, as you obviously
# wouldn't want random X windows getting moved over the TV picture.

A note on unloading the fb driver:

If you want to be able to unload the framebuffer driver (and you aren't
already using fbcon),  add this to your lilo config:

video=vc:x-y

where x is the first fb device to allocate and y is the second. If you 
already have a fb driver loaded, fiddle with the numbers so all the consoles
are already allocated. For me, i just set it to 0-0, ie:

in lilo.conf:

image=/vmlinuz
        label=linux
        read-only
        append="root=/dev/hda1 video=vc:0-0"

--OR--
on bootup, do this
LILO: linux video=vc:0-0

according to how i read /usr/src/linux/drivers/video/fbmem.c and
/usr/src/linux/drivers/char/console.c, that should disable the
console hijacks, and allow you to unload the driver.

-tmk
#
#
#
#
#
#
#
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/fb.h>
#include <linux/console.h>
#include <linux/bitops.h>
#include <linux/pagemap.h>

#include <asm/io.h>
#include <asm/ioctl.h>

#ifdef CONFIG_MTRR
#include <asm/mtrr.h>
#endif /* CONFIG_MTRR */

#include "ivtv-driver.h"
#include "ivtv-queue.h"
#include "ivtv-fileops.h"
#include "ivtv-mailbox.h"
#include "ivtv-cards.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#include <video/fbcon.h>
#include <video/fbcon-cfb32.h>
#endif
#include "ivtv-osd.h"

typedef unsigned long uintptr_t;

static int ivtvfb_setup(struct ivtv *itv);

/*
 * card parameters
 */

static int ivtv_fb_card_id;
static int osd_init;


/* Generic utility functions */
static int ivtvfb_sleep_timeout(int timeout, int intr)
{
	int sleep = timeout;
	int ret = 0;

	do {
		if (intr)
			set_current_state(TASK_INTERRUPTIBLE);
		else
			set_current_state(TASK_UNINTERRUPTIBLE);

		sleep = schedule_timeout(sleep);

		if (intr)
			if ((ret = signal_pending(current)))
				break;
	} while (sleep);
	return ret;
}

/*
 * ivtv API calls for framebuffer related support
 */

static int ivtv_api_fb_get_framebuffer(struct ivtv *itv, u32 * fbbase,
				       u32 * fblength)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int rc;

	rc = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		      CX2341X_OSD_GET_FRAMEBUFFER, &result, 0, &data[0]);
	*fbbase = data[0];
	*fblength = data[1];
	return rc;
}

static int ivtv_api_fb_get_pixel_format(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_GET_PIXEL_FORMAT, &result, 0, &data[0]);
	return data[0];
}

static int ivtv_api_fb_set_pixel_format(struct ivtv *itv, int format)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	data[0] = format;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_SET_PIXEL_FORMAT, &result, 1, &data[0]);
	return result;
}

#if 0
static int ivtv_api_fb_get_status(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_OSD_GET_STATE,
		 &result, 0, &data[0]);

	/* bit 1 is on/off bit */
	if (data[0] & 1)
		return 1;
	else
		return 0;
}
#endif

static int ivtv_api_fb_get_state(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_OSD_GET_STATE,
		 &result, 0, &data[0]);
	return data[0];
}

static int ivtv_api_fb_set_state(struct ivtv *itv, int enabled)
{
	u32 params[IVTV_MBOX_MAX_DATA], result;
	params[0] = enabled;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_OSD_SET_STATE,
		 &result, 1, &params[0]);
	return result;
}

static int ivtv_api_fb_set_framebuffer_window(struct ivtv *itv,
					      int left, int top,
					      int width, int height)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	
        data[0] = width;
	data[1] = height;
	data[2] = left;
	data[3] = top;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_SET_FRAMEBUFFER_WINDOW, &result, 4, &data[0]);
	return result;
}

static int ivtv_api_fb_get_osd_coords(struct ivtv *itv,
				      struct ivtv_osd_coords *osd)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_OSD_GET_OSD_COORDS,
		 &result, 0, &data[0]);

	osd->offset = data[0] - itv->osd_info->video_rbase;
	osd->max_offset = itv->osd_info->display_width * itv->osd_info->display_height * 4;
	osd->pixel_stride = data[1];
	osd->lines = data[2];
	osd->x = data[3];
	osd->y = data[4];

	return result;
}

static int ivtv_api_fb_set_osd_coords(struct ivtv *itv, const struct ivtv_osd_coords
				      *osd)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	data[0] = osd->offset + itv->osd_info->video_rbase;
	data[1] = osd->pixel_stride;
	data[2] = osd->lines;
	data[3] = osd->x;
	data[4] = osd->y;

	/* FIXME maybe wait on vsync? */
	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_OSD_SET_OSD_COORDS,
		 &result, 5, &data[0]);

	return result;
}

static int ivtv_api_fb_get_screen_coords(struct ivtv *itv, struct rectangle *r)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_GET_SCREEN_COORDS, &result, 0, &data[0]);

	r->x0 = data[0];
	r->y0 = data[1];
	r->x1 = data[2];
	r->y1 = data[3];

	return result;
}

static int ivtv_api_fb_set_screen_coords(struct ivtv *itv,
					 const struct rectangle *r)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	data[0] = r->x0;
	data[1] = r->y0;
	data[2] = r->x1;
	data[3] = r->y1;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_SET_SCREEN_COORDS, &result, 4, &data[0]);
	return result;
}

static int ivtv_api_fb_get_global_alpha(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_GET_GLOBAL_ALPHA, &result, 0, &data[0]);
	return data[1];
}

static int ivtv_api_fb_set_global_alpha(struct ivtv *itv,
					int enable_global,
					int alpha, int enable_local)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	/* Save settings if Firmware reload */
	itv->osd_info->global_alpha = alpha;
	itv->osd_info->local_alpha_state = enable_local;
	itv->osd_info->global_alpha_state = enable_global;

	data[0] = enable_global;
	data[1] = alpha;
	data[2] = !enable_local;
	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_SET_GLOBAL_ALPHA, &result, 3, &data[0]);

	return result;
}

static void  ivtv_api_fb_set_colorKey(struct ivtv *itv,int state,  uint32_t color)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

        itv->osd_info->color_key_state = state;
        itv->osd_info->color_key = color;
	data[0] = state;
	data[1] = color;
	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_SET_CHROMA_KEY, &result, 2, &data[0]);

}
static int ivtv_api_fb_get_flicker_state(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_GET_FLICKER_STATE, &result, 0, &data[0]);
	return data[0];
}

static int ivtv_api_fb_set_flicker_state(struct ivtv *itv, int enabled)
{
	u32 params[IVTV_MBOX_MAX_DATA], result;

	params[0] = enabled;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
		 CX2341X_OSD_SET_FLICKER_STATE, &result, 1, &params[0]);
	return result;
}

/* currently unused */
static int ivtv_api_fb_blt_fill(struct ivtv *itv, int rasterop,
				int alpha_mode, int alpha_mask_mode,
				int width, int height, int destmask,
				u32 destaddr, int deststride, u32 value)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;

	data[0] = rasterop;
	data[1] = alpha_mode;
	data[2] = alpha_mask_mode;
	data[3] = width;
	data[4] = height;
	data[5] = destmask;
	data[6] = destaddr;
	data[7] = deststride;
	data[8] = value;

	result =
	    ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_OSD_BLT_FILL,
		     &result, 9, &data[0]);
	return result;
}

#ifdef UNUSED
static int ivtv_api_fb_blt_copy(struct ivtv *itv, int rasterop,
				int alpha_mode, int alpha_mask_mode,
				int width, int height, int destmask,
				u32 destaddr, int deststride,
				int sourcestride, int sourceaddr)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_OSD_DEBUG_INFO(
		       "ivtv_api_fb_blt_copy: width = %d, height = %d, destaddr = %d, deststride = %d, sourcestride = %d, sourceaddr = %d\n",
		       width, height, destaddr, deststride, sourcestride,
		       sourceaddr);

	data[0] = rasterop;
	data[1] = alpha_mode;
	data[2] = alpha_mask_mode;
	data[3] = width;
	data[4] = height;
	data[5] = destmask;
	data[6] = destaddr;
	data[7] = deststride;
	data[8] = sourcestride;
	data[9] = sourceaddr;

	result =
	    ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, CX2341X_OSD_BLT_COPY,
		     &result, 10, &data[0]);
	return result;
}
#endif

MODULE_PARM(ivtv_fb_card_id, "i");
MODULE_PARM_DESC(ivtv_fb_card_id,
		 "ID number of ivtv card to use as framebuffer device (0-7)");
MODULE_PARM(osd_init, "i");
MODULE_PARM_DESC(osd_init,
		 "specify frame buffer initialisation\n"
                 "\t\t\t0=black\n"
                 "\t\t\t1=uninitialised\n"
                 "\t\t\tdefault black");

MODULE_AUTHOR("Kevin Thayer, Chris Kennedy, Hans Verkuil, John Harvey");
MODULE_LICENSE("GPL");

/* --------------------------------------------------------------------- */

static struct fb_var_screeninfo ivtvfb_defined = {
	0, 0, 0, 0,		/* W,H, W, H (virtual) load xres,xres_virtual */
	0, 0,			/* virtual -> visible no offset */
	32,			/* depth -> load bits_per_pixel */
	0,			/* greyscale ? */
	{0, 0, 0},		/* R */
	{0, 0, 0},		/* G */
	{0, 0, 0},		/* B */
	{0, 0, 0},		/* transparency */
	0,			/* standard pixel format */
	FB_ACTIVATE_NOW,
	-1, -1,
	0,
	0L, 0L, 0L, 0L, 0L,
	0L, 0L, 0,		/* No sync info */
	FB_VMODE_NONINTERLACED,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	{0, 0, 0, 0, 0, 0}
#else
	0,
	{0, 0, 0, 0, 0}
#endif
};

static struct fb_info fb_info;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
static struct display disp;
static union {
	u32 cfb32[16];
} fbcon_cmap;
static struct display_switch ivtvfb_sw;
#endif

#ifdef CONFIG_MTRR
static int mtrr = 1;		//++MTY
static unsigned long fb_start_aligned_physaddr;	/* video_base rounded down as required by hardware MTRRs */
static unsigned long fb_end_aligned_physaddr;	/* video_base rounded up as required by hardware MTRRs */
#endif /* CONFIG_MTRR */

/* --------------------------------------------------------------------- */
static int _ivtvfb_set_var(struct ivtv *itv, struct fb_var_screeninfo *var)
{
	IVTV_OSD_DEBUG_INFO("_ivtvfb_set_var\n");

	if (var->xres != ivtvfb_defined.xres ||
	    var->yres != ivtvfb_defined.yres ||
	    var->xres_virtual != ivtvfb_defined.xres_virtual ||
	    var->yres_virtual != ivtvfb_defined.yres_virtual ||
	    var->xoffset ||
	    var->bits_per_pixel != ivtvfb_defined.bits_per_pixel ||
	    var->nonstd) {
		return -EINVAL;
	}
	return 0;

}
static int _ivtvfb_get_fix(struct ivtv *itv, struct fb_fix_screeninfo *fix)
{
	memset(fix, 0, sizeof(struct fb_fix_screeninfo));
	strcpy(fix->id, "cx23415 TV out");
	fix->smem_start = itv->osd_info->video_pbase;
	fix->smem_len = itv->osd_info->video_buffer_size;
	fix->type = FB_TYPE_PACKED_PIXELS;
	fix->visual = FB_VISUAL_TRUECOLOR;
	fix->xpanstep = 0;
	fix->ypanstep = 0;
	fix->ywrapstep = 0;
	fix->line_length = itv->osd_info->display_byte_stride;
	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
static int ivtvfb_update_var(int con, struct fb_info *info)
{
	IVTV_OSD_DEBUG_INFO("ivtvfb_update_var\n");
	return 0;
}

static int ivtvfb_get_fix(struct fb_fix_screeninfo *fix, int con,
			  struct fb_info *info)
{
	IVTV_OSD_DEBUG_INFO("ivtvfb_get_fix\n");
	return _ivtvfb_get_fix(fix);
}

static int ivtvfb_get_var(struct fb_var_screeninfo *var, int con,
			  struct fb_info *info)
{
	IVTV_OSD_DEBUG_INFO("ivtvfb_get_var\n");
	if (con == -1)
		memcpy(var, &ivtvfb_defined, sizeof(struct fb_var_screeninfo));
	else
		*var = fb_display[con].var;
	return 0;
}

static void ivtvfb_set_disp(int con)
{
	struct fb_fix_screeninfo fix;
	struct display *display;
	struct display_switch *sw;

	IVTV_OSD_DEBUG_INFO("ivtvfb_set_disp\n");
	if (con >= 0)
		display = &fb_display[con];
	else
		display = &disp;	/* used during initialization */

	ivtvfb_get_fix(&fix, con, 0);

	memset(display, 0, sizeof(struct display));
	display->screen_base = itv->osd_info->video_vbase;
	display->visual = fix.visual;
	display->type = fix.type;
	display->type_aux = fix.type_aux;
	display->ypanstep = fix.ypanstep;
	display->ywrapstep = fix.ywrapstep;
	display->line_length = fix.line_length;
	display->next_line = fix.line_length;
	display->can_soft_blank = 0;
	display->inverse = 0;
	ivtvfb_get_var(&display->var, -1, &fb_info);

	sw = &fbcon_cfb32;
	display->dispsw_data = fbcon_cmap.cfb32;
	memcpy(&ivtvfb_sw, sw, sizeof(*sw));
	display->dispsw = &ivtvfb_sw;
	display->scrollmode = SCROLL_YREDRAW;
	ivtvfb_sw.bmove = fbcon_redraw_bmove;
}

static int ivtvfb_set_var(struct fb_var_screeninfo *var, int con,
			  struct fb_info *info)
{
	int first = 0;
	IVTV_OSD_DEBUG_INFO("ivtvfb_set_var\n");
	if (con >= 0)
		first = 1;

	if (_ivtvfb_set_var(var)) {
		if (first) {
			IVTV_OSD_ERR
			    ("changing the video mode is not supported\n");
			first = 0;
		}
		return -EINVAL;
	}

	if ((var->activate & FB_ACTIVATE_MASK) == FB_ACTIVATE_TEST)
		return 0;

	if (var->yoffset)
		return -EINVAL;
	return 0;
}

/*
 * ivtv never uses a colormap: it is always straight RGBA 8:8:8:8...
 */
static int ivtvfb_get_cmap(struct fb_cmap *cmap, int kspc, int con,
			   struct fb_info *info)
{
	return 0;
}

static int ivtvfb_set_cmap(struct fb_cmap *cmap, int kspc, int con,
			   struct fb_info *info)
{
	return 0;
}

#else
static int ivtvfb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct ivtv *itv = (struct ivtv *) info->par;
	IVTV_OSD_DEBUG_WARN ("ivtvfb_check_var\n");
	return (_ivtvfb_set_var(itv, &info->var));
}
static int ivtvfb_set_par(struct fb_info *info)
{
	struct ivtv *itv = (struct ivtv *) info->par;
	IVTV_OSD_DEBUG_WARN ("ivtvfb_set_par\n");
	return (_ivtvfb_set_var(itv, &info->var));
}
static int ivtvfb_setcolreg(unsigned regno, unsigned red, unsigned green,
			    unsigned blue, unsigned transp,
			    struct fb_info *info)
{
	return (0);
}

static void ivtv_fb_fillrect(struct fb_info *info,
			     const struct fb_fillrect *rect)
{
}
static void ivtv_fb_copyarea(struct fb_info *info,
			     const struct fb_copyarea *region)
{
}
static void ivtv_fb_imageblit(struct fb_info *info,
			      const struct fb_image *image)
{
}
static int ivtv_fb_cursor(struct fb_info *info, struct fb_cursor *cursor)
{
	return 0;
}
static void ivtv_fb_rotate(struct fb_info *info, int angle)
{
}
static int ivtv_fb_sync(struct fb_info *info)
{
	return 0;
}

#endif

static int ivtv_fb_blt_copy(struct ivtv *itv, int x, int y, int width,
			    int height, int source_offset, int source_stride)
{
	return -EINVAL;
#ifdef UNUSED
	int rc;
	unsigned long destaddr = ((y * itv->osd_info->display_width) + x) * 4;

	source_offset += shadow_framebuf_offset;

	rc = ivtv_api_fb_blt_copy(itv, 0xa, 0x1, 0x0, width, height,
				  0xffffffff, destaddr, itv->osd_info->display_width,
				  source_stride, source_offset);
	return rc;
#endif
}

static int ivtv_fb_blt_fill(struct ivtv *itv,
			    struct ivtvfb_ioctl_blt_fill_args *args)
{
	int rc;
	unsigned long destaddr =
	    IVTV_DEC_MEM_START + itv->osd_info->video_rbase +
	    (((args->y * itv->osd_info->display_width) + args->x) * 4);

	IVTV_OSD_DEBUG_INFO(
		       "ivtv_fb_blt_fill op %d mode %d mask %d width %d hight %d\n",
		       args->rasterop, args->alpha_mode, args->alpha_mask,
		       args->width, args->height);

	IVTV_OSD_DEBUG_INFO(
		       "ivtv_fb_blt_fill destMask %x, destAddr %lx stride %d color %x\n",
		       args->destPixelMask, destaddr, itv->osd_info->display_width,
		       args->colour);

	rc = ivtv_api_fb_blt_fill(itv, args->rasterop, args->alpha_mode,
				  args->alpha_mask, args->width, args->height,
				  args->destPixelMask, destaddr, itv->osd_info->display_width,
				  args->colour);
	return rc;
}

int ivtvfb_prep_dec_dma_to_device(struct ivtv *itv,
				  unsigned long ivtv_dest_addr, char *userbuf,
				  int size_in_bytes)
{
	struct ivtv_stream *stream = &itv->streams[IVTV_DEC_STREAM_TYPE_OSD];
	u32 data[IVTV_MBOX_MAX_DATA];
	int ret = 0;

	/* Map User DMA */
	if (ivtv_prep_user_dma(itv,
			       &stream->udma, ivtv_dest_addr, userbuf,
			       size_in_bytes) <= 0) {
		IVTV_OSD_DEBUG_WARN(
			       "ivtvfb_prep_dec_dma_to_device, "
			       "Error with get_user_pages: %d bytes, %d pages returned\n",
			       size_in_bytes, stream->udma.page_count);

		/* get_user_pages must have failed completely */
		return -EIO;
	}

	IVTV_OSD_DEBUG_INFO(
		       "ivtvfb_prep_dec_dma_to_device, %d bytes, %d pages\n",
		       size_in_bytes, stream->udma.page_count);

	/* Decoder DMA State */
	itv->dec_dma_stat.last_xfer = stream->udma.sg_size;
	atomic_set(&itv->dec_dma_stat.type, 1);

	data[0] = stream->udma.SG_handle;
	data[1] = stream->udma.sg_size;
	data[2] = 0x01;

	IVTV_OSD_DEBUG_INFO(
		       "Sched OSD dma: addr: 0x%08x, array_size 0x%08x, type 0x%08x\n",
		       data[0], data[1], data[2]);

	down(&itv->DMA_lock);
	if (dma_to_device(itv, stream, stream->udma.SG_handle, 0) != 0) {
		IVTV_OSD_DEBUG_WARN("Error OSD User DMA\n");
	}
	up(&itv->DMA_lock);

	/* Unmap Last DMA Xfer */
	if (ivtv_unmap_user_dma(itv, &stream->udma) == 1) {
		IVTV_OSD_DEBUG_WARN( "Error: unmap osd dma\n");
	}

	return ret;
}

int ivtvfb_prep_osd_dma_to_device(struct ivtv *itv,
				  unsigned long ivtv_dest_addr, char *userbuf,
				  int size_in_bytes, int stream_type)
{
	struct ivtv_stream *osd_stream = &itv->streams[stream_type];
	struct ivtv_buffer *buf;
	int bytes_left = size_in_bytes;
	int cur_buf = 0;
	int offset = 0;
	int ret = 0;
	int i;

	/* Decoder DMA State */
	itv->dec_dma_stat.last_xfer = size_in_bytes;
	atomic_set(&itv->dec_dma_stat.type, 1);

	while (bytes_left) {
		/* Get Buffers for DMA xfer */
		for (i = 0;
		     bytes_left
		     && (buf =
			 ivtv_deq_buf_lock(itv, &osd_stream->free_q, osd_stream));
		     i++) {
			/* Calculate Buffer size and Bytes to fill */
			if (bytes_left >= buf->buffer.length) {
				bytes_left -= buf->buffer.length;
				cur_buf = buf->buffer.length;
			} else {
				cur_buf = bytes_left;
				bytes_left = 0;
			}
			buf->buffer.bytesused = cur_buf;
			buf->readpos = cur_buf;
			buf->ts = jiffies;

			/* Fill Buffer */
			if (copy_from_user(((char *)buf->buffer.m.userptr),
					   (char *)((uintptr_t) userbuf +
						    offset), cur_buf)) {
				ivtv_enq_buf_lock(&osd_stream->free_q, buf,
						  osd_stream);
				ret = -EFAULT;
				break;
			}
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
			pci_dma_sync_single_for_cpu((struct pci_dev *)itv->dev,
						    osd_stream->SG_handle,
						    sizeof(struct
							   ivtv_SG_element) *
						    (osd_stream->buf_min /
						     osd_stream->bufsize),
						    osd_stream->dma);
#else
			/* Sync DMA Buffer */
			pci_dma_sync_single(itv->dev,
					    osd_stream->SG_handle,
					    sizeof(struct ivtv_SG_element) *
					    (osd_stream->buf_min / osd_stream->bufsize),
					    osd_stream->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */
			/* Buffer Addr */
			osd_stream->SGarray[i].src = buf->dma_handle;

			/* Decoder Addr */
			osd_stream->SGarray[i].dst = ivtv_dest_addr + offset;
			/* Buffer size */
			osd_stream->SGarray[i].size = buf->buffer.bytesused;

			/* put into DMA Queue */
			ivtv_enq_buf_lock(&osd_stream->dma_q, buf, osd_stream);

			/* Increment next xfer offset */
			offset += cur_buf;
		}

		if (ret || !i) {
			if (!ret)
				ret = -ENOMEM;
			break;
		}

		/* Set SG Length */
		osd_stream->SG_length = i;

		/* Set Last Element Bit */
		osd_stream->SGarray[i - 1].size |= 0x80000000;

		/* Sync SG Array */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8)
		pci_dma_sync_single_for_device((struct pci_dev *)itv->dev,
					       osd_stream->SG_handle,
					       sizeof(struct ivtv_SG_element) *
					       (osd_stream->buf_min /
						osd_stream->bufsize), osd_stream->dma);
#else
		pci_dma_sync_single(itv->dev,
				    osd_stream->SG_handle,
				    sizeof(struct ivtv_SG_element) *
				    (osd_stream->buf_min / osd_stream->bufsize),
				    osd_stream->dma);
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 8) */

		IVTV_OSD_DEBUG_INFO(
			       "Sched OSD dma: links 0x%08x, bytes 0x%08x, type %d\n",
			       osd_stream->SG_length, size_in_bytes, 1);

		down(&itv->DMA_lock);
		if (dma_to_device(itv, osd_stream, osd_stream->SG_handle, 0) != 0) {
			IVTV_OSD_DEBUG_WARN("Error OSD BUFFER DMA\n");
		}
		up(&itv->DMA_lock);

		/* Gather Buffers from DMA xfer to read */
		ivtv_TO_DMA_done(itv, osd_stream->type);
	}

	return ret;
}

static int ivtv_fb_prep_frame_buf(struct ivtv *itv,
				  unsigned long destaddr, void *srcaddr,
				  int count)
{
	int rc = 0;
	struct ivtv_stream *stream =	/* Use Decoder Stream for locking */
	    &itv->streams[IVTV_DEC_STREAM_TYPE_OSD];
	unsigned long then;
	DECLARE_WAITQUEUE(wait, current);

	/* If needing to re-setup the OSD */
	if (test_and_clear_bit(OSD_RESET_NEEDED, &itv->r_flags))
		ivtvfb_setup(itv);

	then = jiffies;
	add_wait_queue(&stream->waitq, &wait);
	set_bit(IVTV_F_S_OSD_NEEDS_DATA, &stream->s_flags);
	do {
		set_current_state(TASK_INTERRUPTIBLE);

		/* Lock Decoder */
		if ((readl((unsigned char *)itv->reg_mem +
				   IVTV_REG_DMASTATUS) & 0x01)
		    &&
		    !(readl
		      ((unsigned char *)itv->reg_mem +
		       IVTV_REG_DMASTATUS) & 0x14)
		    && !test_and_set_bit(IVTV_F_S_DMAP, &stream->s_flags)) {
			break;
		}
		IVTV_OSD_DEBUG_DEC(
			   "DMA Reg Status: 0x%08x needs 0x%08x has 0x%08x.\n",
			   readl(itv->dec_mem + IVTV_REG_DEC_READY),
			   readl(itv->dec_mem + IVTV_REG_DEC_NEEDED),
			   readl(itv->dec_mem + IVTV_REG_DEC_FILL));

		if (ivtvfb_sleep_timeout(HZ / 100, 1)) {
                        rc = -ERESTARTSYS;
                        break;
		}

                if ((jiffies - then) > (HZ*3)) {
                        IVTV_OSD_DEBUG_WARN("Timeout waiting.\n");
                        rc = -EINTR;
                        break;
                }
	} while (!rc);
	clear_bit(IVTV_F_S_OSD_NEEDS_DATA, &stream->s_flags);
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&stream->waitq, &wait);

	if (rc)	/* Myth breaks when not returning the errors, misses things */
		return rc;

	/* OSD Address to send DMA to */
	destaddr = IVTV_DEC_MEM_START + itv->osd_info->video_rbase + destaddr;
	/* Fill Buffers */
	if (0 != (rc = ivtvfb_prep_osd_dma_to_device(itv,
						     destaddr, (char *)srcaddr,
						     count,
						     IVTV_DEC_STREAM_TYPE_OSD)))
	{
		IVTV_OSD_DEBUG_WARN(
			       "Error: prep osd dma to device rc=%d\n", rc);
	}
	unlock_TO_dma(itv, stream->type);

	return rc;
}

static int ivtv_fb_prep_frame(struct ivtv *itv,
			      unsigned long destaddr, void *srcaddr, int count)
{
	int rc = 0;
	struct ivtv_stream *stream =	/* Use Decoder Stream for locking */
	    &itv->streams[IVTV_DEC_STREAM_TYPE_OSD];
	unsigned long then;
	DECLARE_WAITQUEUE(wait, current);

	/* If needing to re-setup the OSD */
	if (test_and_clear_bit(OSD_RESET_NEEDED, &itv->r_flags)) {
		ivtvfb_setup(itv);
		unlock_TO_dma(itv, stream->type);
	}

	then = jiffies;
	add_wait_queue(&stream->udma.waitq, &wait);
	set_bit(IVTV_F_S_OSD_NEEDS_DATA, &stream->s_flags);
	do {
		set_current_state(TASK_INTERRUPTIBLE);

		/* Lock Decoder */
		if ((readl((unsigned char *)itv->reg_mem +
				   IVTV_REG_DMASTATUS) & 0x01)
		    &&
		    !(readl
		      ((unsigned char *)itv->reg_mem +
		       IVTV_REG_DMASTATUS) & 0x14)
		    && !test_bit(DMA_IN_USE, &stream->udma.u_flags)
		    && !test_and_set_bit(IVTV_F_S_DMAP, &stream->s_flags)) {
			if (!test_and_set_bit
			    (DMA_IN_USE, &stream->udma.u_flags))
				break;
			else
				clear_bit(IVTV_F_S_DMAP, &stream->s_flags);
		}
		IVTV_OSD_DEBUG_DEC(
			   "DMA Reg Status: 0x%08x needs 0x%08x has 0x%08x.\n",
			   readl(itv->dec_mem + IVTV_REG_DEC_READY),
			   readl(itv->dec_mem + IVTV_REG_DEC_NEEDED),
			   readl(itv->dec_mem + IVTV_REG_DEC_FILL));

		if (ivtvfb_sleep_timeout(HZ / 100, 1)) {
                        rc = -ERESTARTSYS;
                        break;
		}

                if ((jiffies - then) > (HZ*3)) {
                        IVTV_OSD_DEBUG_WARN("Timeout waiting.\n");
			rc = -EINTR;
                        break;
                }
	} while (!rc);
	clear_bit(IVTV_F_S_OSD_NEEDS_DATA, &stream->s_flags);
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&stream->udma.waitq, &wait);

	if (rc)	/* Myth breaks when not returning the errors, misses things */
		return rc;

	/* OSD Address to send DMA to */
	destaddr = IVTV_DEC_MEM_START + itv->osd_info->video_rbase + destaddr;

	/* Fill Buffers */
	if (0 != (rc = ivtvfb_prep_dec_dma_to_device(itv,
						     destaddr, (char *)srcaddr,
						     count))) {
		IVTV_OSD_DEBUG_WARN(
			       "Error: prep osd dma to device rc=%d\n", rc);
	}
	unlock_TO_dma(itv, stream->type);

	return rc;
}

static int ivtv_prep_frame(struct ivtv *itv, int cmd, void *source, unsigned long dest_offset, int count)
{
	int ret = 0;

        /* Check Count */
        if (count < IVTV_DMA_OSD_ALIGN_XFER) {
                IVTV_OSD_DEBUG_WARN(
                           "small xfer offset 0x%08lx source 0x%08lx size %d\n",
                           dest_offset, (unsigned long)source, count);
                return 0;
        }

        /* Align */
        if (cmd != IVTVFB_IOCTL_PREP_FRAME_BUF &&
                        (count % IVTV_DMA_OSD_ALIGN_XFER)) {
                count = count - (count % IVTV_DMA_OSD_ALIGN_XFER);
        }

        if (count <= 0) {
                IVTV_OSD_DEBUG_WARN("Size is %d!!!\n", count);
                return 0;	/* Nothing to do */
        }

        /* Bad Offset */
        if ((unsigned int)dest_offset < 0)
                dest_offset = 0;
        else if (dest_offset >= itv->osd_info->video_buffer_size) {
                IVTV_OSD_DEBUG_WARN(
                               "Offset %ld is greater than buffer!!!\n",
                               dest_offset);
                if (itv->osd_info->video_buffer_size > count)
                        dest_offset = itv->osd_info->video_buffer_size - count;
                if (dest_offset < 0)
                        dest_offset = 0;
                else if (dest_offset >= itv->osd_info->video_buffer_size) {
                        IVTV_OSD_DEBUG_WARN(
                                "Count %d Offset %ld is greater than buffer\n",
                                count, dest_offset);
                        return 0;
                }
        }

        /* Check Total FB Size */
        if (((dest_offset + count) > itv->osd_info->video_buffer_size) || (count > itv->osd_info->video_buffer_size)) {
                IVTV_OSD_DEBUG_WARN(
                        "Size is overflowing the framebuffer %ld, "
                        "only %d available\n",
                        (dest_offset + count), itv->osd_info->video_buffer_size);

                return -E2BIG;
        }

        /* Check Source */
        if (!access_ok(VERIFY_READ, source + dest_offset, count)) {
                IVTV_OSD_DEBUG_WARN(
                           "Invalid userspace pointer!!! 0x%08lx\n",
                           (unsigned long)source);

                IVTV_OSD_DEBUG_WARN(
                           "access_ok() failed for offset 0x%08lx source 0x%08lx count %d\n",
                           dest_offset, (unsigned long)source,
                           count);
                return -EINVAL;
        }

        /* Send DMA */
        if (cmd == IVTVFB_IOCTL_PREP_FRAME) {
                /*if (wait_event_interruptible(itv->vsync_w_osd,
                        atomic_read(&itv->dec_dma_stat.vsync_flag_osd))) {
                        ret = -ERESTARTSYS;
                } else {*/
                        atomic_set(&itv->dec_dma_stat.vsync_flag_osd, 0);
                        ret = ivtv_fb_prep_frame(itv, dest_offset,
                                               source, count);
                //}
        } else if (cmd == IVTVFB_IOCTL_PREP_FRAME_BUF) {
                if (wait_event_interruptible(itv->vsync_w_osd,
                                atomic_read(&itv->dec_dma_stat.vsync_flag_osd)))
                {
                        ret = -ERESTARTSYS;
                } else {
                        atomic_set(&itv->dec_dma_stat.vsync_flag_osd, 0);
                        ret = ivtv_fb_prep_frame_buf(itv,
                                                   dest_offset, source, count);
                }
        }

        /* Failed */
        if (ret) {
                if (ret != -ERESTARTSYS) {
                        IVTV_OSD_DEBUG_WARN(
                                   "DMA xfer from %8p of %d bytes "
                                   "failed with (%d) offset = 0x%0lx, total %d\n",
                                   source, count, ret, dest_offset,
                                   (count + (unsigned int)dest_offset));
                } else {
                        IVTV_OSD_DEBUG_INFO(
                                   "DMA xfer interrupted so restarting\n");
                }
        }

        return ret;
}

/* Temporarily disabled to work around FC5 kernel version madness.
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
static int ivtv_fb_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		  unsigned long arg, int con, struct fb_info *info)
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
static int ivtv_fb_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		  unsigned long arg, struct fb_info *info)
#else
*/
static int ivtv_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
/*
#endif
#endif
*/
{

	int rc;

	struct ivtv *itv = (struct ivtv *) info->par;

	switch (cmd) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	case 0x7777: {
		while (MOD_IN_USE)
			MOD_DEC_USE_COUNT;
		MOD_INC_USE_COUNT;
		return 0;
	}
#endif
	case IVTVFB_IOCTL_GET_STATE: {
		struct ivtvfb_ioctl_state_info state;

		state.status = (ivtv_api_fb_get_state(itv) & 0x7);
		state.status |= (ivtv_api_fb_get_flicker_state(itv) << 3);
		state.status |= IVTVFB_STATUS_ENABLED;
		state.alpha = ivtv_api_fb_get_global_alpha(itv);
		IVTV_OSD_DEBUG_IOCTL(
			       "IVTVFB_IOCTL_GET_STATE: status = %lu, alpha = %lu\n",
			       state.status, state.alpha);
		if (copy_to_user((void *)arg, &state, sizeof(state)))
			return -EFAULT;
		return 0;
	}

	case IVTVFB_IOCTL_SET_STATE: {
		struct ivtvfb_ioctl_state_info state;

		if (copy_from_user(&state, (void *)arg, sizeof(state)))
			return -EFAULT;
		IVTV_OSD_DEBUG_IOCTL(
			"IVTVFB_IOCTL_SET_STATE: status = %lu, alpha = %lu\n",
			state.status, state.alpha);

		/* FIXME: the OSD seems to turn itself off, so we enable it no
		   matter what the current state is, something odd going on there. */
		ivtv_api_fb_set_state(itv,
	                (state.status & IVTVFB_STATUS_ENABLED) ? 1 : 1);
		ivtv_api_fb_set_global_alpha(itv,
			(state.status & IVTVFB_STATUS_GLOBAL_ALPHA) ? 1 : 0, 
                        state.alpha, (state.status & IVTVFB_STATUS_LOCAL_ALPHA) ? 1 : 0);
		ivtv_api_fb_set_flicker_state(itv,
			(state.status & IVTVFB_STATUS_FLICKER_REDUCTION) ? 1 : 0);
		IVTV_OSD_DEBUG_IOCTL("new state = %d, global alpha now = %d\n",
			ivtv_api_fb_get_state(itv), ivtv_api_fb_get_global_alpha(itv));
		return 0;
	}

	case IVTVFB_IOCTL_PREP_FRAME_BUF:
	case IVTVFB_IOCTL_PREP_FRAME: {
		struct ivtvfb_ioctl_dma_host_to_ivtv_args args;

		/* Just use buffers, user space buffers are too small */
		//cmd = IVTVFB_IOCTL_PREP_FRAME_BUF;

		IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_PREP_FRAME\n");

		if (copy_from_user(&args, (void *)arg, sizeof(args)))
			return -EFAULT;

                return ivtv_prep_frame(itv, cmd, args.source, args.dest_offset, args.count);
	}

	case IVTVFB_IOCTL_BLT_COPY: {
		struct ivtvfb_ioctl_blt_copy_args args;

		IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_BLT_COPY\n");
		if (copy_from_user(&args, (void *)arg, sizeof(args)))
			return -EFAULT;

		return ivtv_fb_blt_copy(itv, args.x, args.y, args.width,
					args.height, args.source_stride,
					args.source_offset);
	}

	case IVTVFB_IOCTL_BLT_FILL: {
		struct ivtvfb_ioctl_blt_fill_args args;

		IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_BLT_FILL\n");
		if (copy_from_user(&args, (void *)arg, sizeof(args)))
			return -EFAULT;

		return ivtv_fb_blt_fill(itv, &args);
	}

	case IVTVFB_IOCTL_GET_ACTIVE_BUFFER: {
		struct ivtv_osd_coords bufinfo;

		IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_GET_ACTIVE_BUFFER\n");
		rc = ivtv_api_fb_get_osd_coords(itv, &bufinfo);
		return copy_to_user((void *)arg, &bufinfo, sizeof(bufinfo));
	}

	case IVTVFB_IOCTL_SET_ACTIVE_BUFFER: {
		struct ivtv_osd_coords bufinfo;

		IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_SET_ACTIVE_BUFFER\n");
		if (copy_from_user(&bufinfo, (void *)arg, sizeof(bufinfo)))
			return -EFAULT;
		return ivtv_api_fb_set_osd_coords(itv, &bufinfo);
	}
	case IVTVFB_IOCTL_SET_WINDOW: {
		struct ivtvfb_ioctl_set_window bufinfo;

		IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_SET_WINDOW\n");
		if (copy_from_user(&bufinfo, (void *)arg, sizeof(bufinfo)))
			return -EFAULT;
		return ivtv_api_fb_set_framebuffer_window(itv,
							  bufinfo.left,
							  bufinfo.top,
							  bufinfo.width,
							  bufinfo.height);
	}

	case IVTVFB_IOCTL_GET_COLORKEY: {
                struct ivtvfb_ioctl_colorkey getColorKey;

                IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_GET_COLORKEY\n");
                getColorKey.state = itv->osd_info->color_key_state;
                getColorKey.colorKey  = itv->osd_info->color_key;
                return copy_to_user((void *)arg, &getColorKey, sizeof(getColorKey));
        }

	case IVTVFB_IOCTL_SET_COLORKEY: {
                struct ivtvfb_ioctl_colorkey colorKey; 

                IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_SET_COLORKEY\n");
                if (copy_from_user(&colorKey, (void *)arg, sizeof(colorKey)))
                       return -EFAULT;
                ivtv_api_fb_set_colorKey(itv,colorKey.state, colorKey.colorKey);
                return 0;
        }
	case IVTVFB_IOCTL_GET_FRAME_BUFFER: {
		struct ivtvfb_ioctl_get_frame_buffer getfb;

		IVTV_OSD_DEBUG_IOCTL("IVTVFB_IOCTL_GET_FRAME_BUFFER\n");
		getfb.mem = (void *)itv->osd_info->video_vbase;
		getfb.size = itv->osd_info->video_buffer_size;
		getfb.sizex = itv->osd_info->display_width;
		getfb.sizey = itv->osd_info->display_height;

		return copy_to_user((void *)arg, &getfb, sizeof(getfb));
	}

	default:
		return -EINVAL;
	}
	return 0;
}

static ssize_t
ivtv_fb_write(struct file *file, const char *ubuf, size_t count, loff_t * ppos)
{
	printk(KERN_INFO "ivtv ERROR: ivtv_fb_write - unsupported function\n");
	return -EINVAL;
#ifdef UNSUPPORTED
	unsigned long p = *ppos;
	int err = 0;
	unsigned long flags;

	if (p == 0)
		IVTV_OSD_DEBUG_WARN(
			       "ERROR: Direct writing is not safe for ivtv usage\n");
	return -EINVAL;

	IVTV_OSD_DEBUG_INFO(
		       "count=%zd, size=%d itv->osd.pos = %lu\n",
		       count, itv->osd_info->video_buffer_size, (unsigned long)ppos);

	if (p > itv->osd_info->video_buffer_size)
		return -ENOSPC;
	if (count >= itv->osd_info->video_buffer_size)
		count = itv->osd_info->video_buffer_size;
	if (count + p > itv->osd_info->video_buffer_size) {
		count = itv->osd_info->video_buffer_size - p;
		err = -ENOSPC;
	}
	if (count) {
		char *base_addr;

		base_addr = itv->osd_info->video_vbase;

		spin_lock_irqsave(&itv->DMA_slock, flags);
		memcpy_toio(base_addr + p, (char *)ubuf, count);
		spin_unlock_irqrestore(&itv->DMA_slock, flags);

		*ppos += count;
		err = -EFAULT;
	}
	if (count)
		return count;
	return err;
#endif
}

/* Temporarily disabled to work around FC5 kernel version madness.
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16)
static int ivtv_fb_mmap(struct fb_info *info,
	     struct file *file, struct vm_area_struct *vma)
#else */
static int ivtv_fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
/*#endif*/
{
	struct ivtv *itv = (struct ivtv *) info->par;
	IVTV_OSD_DEBUG_WARN( "MMAP is not safe for ivtv usage\n");
	return -EINVAL;
}

static struct fb_ops ivtvfb_ops = {
      owner:THIS_MODULE,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
      fb_get_fix:ivtvfb_get_fix,
      fb_get_var:ivtvfb_get_var,
      fb_set_var:ivtvfb_set_var,
      fb_get_cmap:ivtvfb_get_cmap,
      fb_set_cmap:ivtvfb_set_cmap,
#else
      fb_check_var:ivtvfb_check_var,
      fb_set_par:ivtvfb_set_par,
      fb_setcolreg:ivtvfb_setcolreg,
      fb_write:ivtv_fb_write,
      fb_fillrect:ivtv_fb_fillrect,
      fb_copyarea:ivtv_fb_copyarea,
      fb_imageblit:ivtv_fb_imageblit,
      fb_cursor:ivtv_fb_cursor,
      fb_rotate:ivtv_fb_rotate,
      fb_sync:ivtv_fb_sync,
#endif
      fb_ioctl:ivtv_fb_ioctl,
      fb_pan_display:NULL,
      fb_mmap:ivtv_fb_mmap,
};

/* 0 unblank, 1 blank, 2 no vsync, 3 no hsync, 4 off */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
static void ivtvfb_blank(int blank, struct fb_info *info)
{
	/* Not supported */
}
#endif

static int ivtvfb_setup(struct ivtv *itv)
{
	int rc;
	struct ivtv_osd_coords osd;
	struct rectangle rect;

	rc = ivtv_api_fb_set_pixel_format(itv, 4);	// 4 = AlphaRGB 8:8:8:8

	IVTV_OSD_DEBUG_INFO("Current pixel format = %d\n",
		       ivtv_api_fb_get_pixel_format(itv));

 	itv->osd_info->display_width = 720;

	if (itv->std & V4L2_STD_625_50) {
 		itv->osd_info->display_height = 576;
	} else {
 		itv->osd_info->display_height = 480;
	}

	/* set number of internal decoder buffers */
	ivtv_vapi(itv,
		  CX2341X_DEC_SET_DISPLAY_BUFFERS, 1, itv->dec_options.decbuffers);

	rc = ivtv_api_fb_get_osd_coords(itv, &osd);
	IVTV_OSD_DEBUG_INFO(
		       "offset = 0x%08x (max offset = 0x%08x), pixel_stride = %d, lines = %d, x = %d, y = %d\n",
		       (u32) osd.offset, (u32) osd.max_offset, osd.pixel_stride,
		       osd.lines, osd.x, osd.y);

	/* setup OSD and screen for PAL */
	if (itv->std & V4L2_STD_625_50) {
		osd.lines = 576;
		rc = ivtv_api_fb_set_osd_coords(itv, &osd);
		if (rc)
			IVTV_OSD_DEBUG_WARN(
				       "failed setting PAL osd\n");

		rect.x0 = 0;
		rect.x1 = 720;
		rect.y0 = 0;
		rect.y1 = 576;
		rc = ivtv_api_fb_set_screen_coords(itv, &rect);
		if (rc)
			IVTV_OSD_DEBUG_WARN(
				       "failed setting PAL screen\n");

		/* Set Window size */
		ivtv_api_fb_set_framebuffer_window(itv, 0, 0, 720, 576);

	} else {		/* Set Window size */
		osd.lines = 480;
		rc = ivtv_api_fb_set_osd_coords(itv, &osd);
		if (rc)
			IVTV_OSD_DEBUG_WARN(
				       "failed setting PAL osd\n");

		rect.x0 = 0;
		rect.x1 = 720;
		rect.y0 = 0;
		rect.y1 = 480;
		rc = ivtv_api_fb_set_screen_coords(itv, &rect);
		if (rc)
			IVTV_OSD_DEBUG_WARN(
				       "failed setting PAL screen\n");

		/* Set Window size */
		ivtv_api_fb_set_framebuffer_window(itv, 0, 0, 720, 480);
	}

	rc = ivtv_api_fb_get_screen_coords(itv, &rect);
	IVTV_OSD_INFO("screen coords: [%d %d] -> [%d %d]\n",
			   rect.x0, rect.y0, rect.x1, rect.y1);

	IVTV_OSD_INFO("original global alpha = %d\n",
			   ivtv_api_fb_get_global_alpha(itv));

	rc = ivtv_api_fb_set_state(itv, 1);	// 1 = enabled
	IVTV_OSD_INFO("current OSD state = %d\n",
			   ivtv_api_fb_get_state(itv));

	ivtv_api_fb_set_global_alpha(itv,
				     itv->osd_info->global_alpha_state,
				     itv->osd_info->global_alpha, itv->osd_info->local_alpha_state);
        ivtv_api_fb_set_colorKey(itv,0,0);
	IVTV_OSD_INFO("new global alpha = %d (%d %d %d)\n",
			   ivtv_api_fb_get_global_alpha(itv),
			   itv->osd_info->global_alpha_state, itv->osd_info->global_alpha,
			   itv->osd_info->local_alpha_state);

	itv->osd_info->display_byte_stride = 4 * osd.pixel_stride;



	return 0;
}

int __init ivtvfb_init(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	struct fb_fix_screeninfo fix;
#endif
	int i;
        
        struct ivtv *itv;

	if ((ivtv_fb_card_id < 0) || (ivtv_fb_card_id >= ivtv_cards_active)) {
		printk(KERN_ERR "ivtv-osd:  ivtv_fb_card_id parameter is out of range (valid range: 0-%d)\n",
		     ivtv_cards_active - 1);
		return -1;
	}

	/* find first tvout enabled card if none is specified */
	if (ivtv_fb_card_id == 0) {
		i = 0;
		while (i < ivtv_cards_active) {
			itv = ivtv_cards[i];
			if (itv && (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT)) {
				ivtv_fb_card_id = i;
				break;
			}
			++i;
		}
	}

	itv = ivtv_cards[ivtv_fb_card_id];
	if (!itv || !(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT)) {
		printk(KERN_ERR "ivtv-osd:  Specified card (id %d) is either not present or does not support TV out\n",
		     ivtv_fb_card_id);
		return -1;
	}

        if (itv->osd_info)
        {
		printk(KERN_ERR "ivtv-osd:  Card %d already initialised\n",
		     ivtv_fb_card_id);
                return -1;
        }

	IVTV_OSD_INFO
	    ("Framebuffer module loaded (attached to ivtv card id %d)\n",
	     ivtv_fb_card_id);
	itv->osd_info = kmalloc(sizeof(struct osd_info), GFP_ATOMIC);
	if (itv->osd_info == 0) {
            printk(KERN_ERR "ivtv-osd:  Failed to allocate memory for osd_info\n");
                return -1;
        }
            
	// Set the startup video information
	
        /*
	 * Normally a 32-bit RGBA framebuffer would be fine, however XFree86's fbdev
	 * driver doesn't understand the concept of alpha channel and always sets
	 * bits 24-31 to zero when using a 24bpp-on-32bpp framebuffer device. We fix
	 * this behavior by enabling the iTVC15's global alpha feature, which causes     
	 * the chip to ignore the per-pixel alpha data and instead use one value (e.g.,
	 * full brightness = 255) for the entire framebuffer. The local alpha is also
	 * disabled in this step.
	 *
	 *++MTY Need to update http://ivtv.sourceforge.net/ivtv/firmware-api.html
	 *      call 0x4b: param[2] says 1 = enable local alpha, when in reality
	 *      it means *disable* local alpha...
	 *
	 */

	// Reset the Alpha status
	itv->osd_info->global_alpha = 255;
	itv->osd_info->global_alpha_state = 1;
	itv->osd_info->local_alpha_state = 0;
        

        /* Setup OSD */
	ivtvfb_setup(itv);

	ivtv_api_fb_get_framebuffer(itv, &(itv->osd_info->video_rbase), &itv->osd_info->video_buffer_size);
	
	itv->osd_info->video_pbase = itv->base_addr + IVTV_DECODER_OFFSET + itv->osd_info->video_rbase;
	itv->osd_info->video_vbase = itv->dec_mem + itv->osd_info->video_rbase;

	if (!itv->osd_info->video_vbase) {
		IVTV_OSD_ERR
		    ("abort, video memory 0x%x @ 0x%lx isn't mapped!!!\n",
		     itv->osd_info->video_buffer_size, itv->osd_info->video_pbase);
		return -EIO;
	}



	IVTV_OSD_INFO("framebuffer at 0x%lx, mapped to 0x%p, size %dk\n",
			   itv->osd_info->video_pbase, itv->osd_info->video_vbase, itv->osd_info->video_buffer_size / 1024);
	IVTV_OSD_INFO("mode is %dx%dx%d, linelength=%d\n",
			   itv->osd_info->display_width, itv->osd_info->display_height, 32, itv->osd_info->display_byte_stride);

	ivtvfb_defined.xres = itv->osd_info->display_width;
	ivtvfb_defined.yres = itv->osd_info->display_height;
	ivtvfb_defined.xres_virtual = itv->osd_info->display_width;
	ivtvfb_defined.yres_virtual = itv->osd_info->display_height;
	ivtvfb_defined.bits_per_pixel = 32;

	/* some dummy values for timing to make fbset happy */
	ivtvfb_defined.pixclock = 10000000 / itv->osd_info->display_width * 1000 / itv->osd_info->display_height;
	ivtvfb_defined.left_margin = (itv->osd_info->display_width / 8) & 0xf8;
	ivtvfb_defined.right_margin = 32;
	ivtvfb_defined.upper_margin = 16;
	ivtvfb_defined.lower_margin = 4;
	ivtvfb_defined.hsync_len = (itv->osd_info->display_width / 8) & 0xf8;
	ivtvfb_defined.vsync_len = 4;

	ivtvfb_defined.red.offset = 0;
	ivtvfb_defined.red.length = 8;
	ivtvfb_defined.green.offset = 8;
	ivtvfb_defined.green.length = 8;
	ivtvfb_defined.blue.offset = 16;
	ivtvfb_defined.blue.length = 8;
	ivtvfb_defined.transp.offset = 24;
	ivtvfb_defined.transp.length = 8;

#ifdef CONFIG_MTRR
	if (mtrr) {
		/* Find the largest power of two that maps the whole buffer */
		int size_shift = 31;
		while (!(itv->osd_info->video_buffer_size & (1 << size_shift))) {
			size_shift--;
		}
		size_shift++;
		fb_start_aligned_physaddr =
		    itv->osd_info->video_pbase & ~((1 << size_shift) - 1);
                //		fb_end_aligned_physaddr =
                //(itv->osd_info->video_pbase + (1 << size_shift) - 1) & ~((1 << size_shift) -
		fb_end_aligned_physaddr = (itv->osd_info->video_pbase+itv->osd_info->video_buffer_size + (1 << size_shift) - 1) & ~((1 << size_shift) -1);
		if (mtrr_add
		    (fb_start_aligned_physaddr,
		     (fb_end_aligned_physaddr - fb_start_aligned_physaddr),
		     MTRR_TYPE_WRCOMB, 1) < 0) {
			IVTV_OSD_ERR
			    ("warning: mtrr_add() failed to add write combining region 0x%08x-0x%08x\n",
			     (unsigned int)fb_start_aligned_physaddr,
			     (unsigned int)fb_end_aligned_physaddr);
		}
	}
#endif /* CONFIG_MTRR */

	fb_info.node = -1;
	fb_info.flags = FBINFO_FLAG_DEFAULT;
	fb_info.fbops = &ivtvfb_ops;
	fb_info.par = itv;
        if (osd_init == 0)
		memset(itv->osd_info->video_vbase, 0, itv->osd_info->display_width * itv->osd_info->display_height*4);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	strcpy(fb_info.modename, "cx23415 TV out");
	fb_info.fontname[0] = '\0';
	fb_info.changevar = NULL;
	fb_info.disp = &disp;
	fb_info.switch_con = NULL;	//&ivtvfb_switch; (fbcon will ignore it then)
	fb_info.updatevar = &ivtvfb_update_var;
	fb_info.blank = &ivtvfb_blank;
	ivtvfb_set_disp(-1);
#else
	_ivtvfb_get_fix(itv,&fix);
	fb_info.var = ivtvfb_defined;
	fb_info.fix = fix;
	fb_info.screen_base = itv->osd_info->video_vbase;
	fb_info.fbops = &ivtvfb_ops;
	fb_alloc_cmap(&fb_info.cmap, 0, 0);
#endif

	if (register_framebuffer(&fb_info) < 0)
		return -EINVAL;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	itv->fb_id = GET_FB_IDX(fb_info.node);

	IVTV_OSD_INFO("fb%d: %s frame buffer device\n",
			   itv->fb_id, fb_info.modename);
#else
	itv->fb_id = fb_info.node;

	IVTV_OSD_INFO("fb%d: %s frame buffer device\n",
			   itv->fb_id, fix.id);
#endif

	/* Allocate DMA */
	ivtv_alloc_user_dma(itv,
			    &itv->streams[IVTV_DEC_STREAM_TYPE_OSD].udma,
			    IVTV_DMA_SG_OSD_ENT);

	if (ivtv_stream_alloc(itv, IVTV_DEC_STREAM_TYPE_OSD)) {
		IVTV_OSD_DEBUG_WARN( "Error allocating OSD Stream\n");
	}

	/* Allow module to unload */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	while (MOD_IN_USE)
		MOD_DEC_USE_COUNT;
	MOD_INC_USE_COUNT;
#endif

	return 0;
}

static void ivtvfb_cleanup(void)
{
	struct ivtv *itv;
	int i;
		
	printk(KERN_INFO "ivtv-osd: Unloading framebuffer module\n");

	for ( i = 0; i < ivtv_cards_active; i++) {
		itv = ivtv_cards[i];
		if (itv && (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT)) {
                    if (itv->osd_info)
                    {
				IVTV_OSD_DEBUG_WARN ("Unregister framebuffer %d\n",i);
#ifdef JOHN
				unregister_framebuffer(&itv->osd_info.ivtvfb_info);
#endif
				// Switch off the OSD
				ivtv_api_fb_set_state(itv, 0);

#ifdef JOHN
				if (itv->osd_info.ivtvfb_info.pseudo_palette)
					kfree(itv->osd_info.ivtvfb_info.pseudo_palette);

#ifdef CONFIG_MTRR
				mtrr_del(-1, itv->osd_info.fb_start_aligned_physaddr,
		  			(itv->osd_info.fb_end_aligned_physaddr - itv->osd_info.fb_start_aligned_physaddr));
#endif /* CONFIG_MTRR */
#endif
				
                                kfree(itv->osd_info);
                                itv->osd_info = NULL;

				/* Free DMA */
				if (&itv->streams[IVTV_DEC_STREAM_TYPE_OSD].udma)
					ivtv_free_user_dma(itv, &itv->streams[IVTV_DEC_STREAM_TYPE_OSD].udma);

				ivtv_stream_free(itv, IVTV_DEC_STREAM_TYPE_OSD);
                    }
                }
                itv->fb_id = -1;
        }

	unregister_framebuffer(&fb_info);
#ifdef CONFIG_MTRR
	mtrr_del(-1, fb_start_aligned_physaddr,
		 (fb_end_aligned_physaddr - fb_start_aligned_physaddr));
#endif /* CONFIG_MTRR */

}

module_init(ivtvfb_init);
module_exit(ivtvfb_cleanup);
