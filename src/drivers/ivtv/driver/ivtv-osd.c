/*
    On Screen Display iTVC15 Framebuffer driver
 
    This module presents the iTVC15 OSD (onscreen display) framebuffer memory 
    as a standard Linux /dev/fb style framebuffer device. The framebuffer has
    a 32 bpp packed pixel format with full alpha channel support. Depending
    on the TV standard configured in the ivtv module at load time, resolution
    is fixed at either 720x480 (NTSC) or 720x576 (PAL).

    Copyright (c) 2003 Matt T. Yourst <yourst@yourst.com>
  
    Derived from drivers/video/vesafb.c
    Portions (c) 1998 Gerd Knorr <kraxel@goldbach.in-berlin.de>

    2.6 kernel port:
    Copyright (C) 2004 Matthias Badaire

    Copyright (C) 2004  Chris Kennedy ckennedy@kmos.org

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
    Identifier  "Hauppauge PVR 350 iTVC15 Framebuffer"
    Driver      "fbdev"
    Option      "fbdev" "/dev/fb1"      # <-- modify if using another device
    BusID "0:10:0"
EndSection

Section "Screen"
  Identifier  "TV Screen"
  Device      "Hauppauge PVR 350 iTVC15 Framebuffer"
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
#endif

#include "ivtv-driver.h"
#include "ivtv-dma.h"
#include "ivtv-queue.h"
#include "ivtv-fileops.h"
#include "ivtv-mailbox.h"
#include "ivtv-cards.h"

#ifndef LINUX26
#include <video/fbcon.h>
#include <video/fbcon-cfb32.h>
#endif

typedef unsigned long uintptr_t;


int ivtvfb_setup(void);
int ivtvfb_sleep_timeout(int timeout, int intr);

/*
 * card parameters
 */

static int ivtv_fb_card_id;

/* Card selected as framebuffer for this module instance: */
static struct ivtv *itv;

/* card */
static unsigned long video_base;	/* physical addr */
static unsigned long video_rel_base;	/* address relative to base of decoder memory */
static int video_size;
static char *video_vbase;		/* mapped */

/* mode */
static int video_width;
static int video_height;
static int video_height_virtual;
static int video_linelength;
static unsigned long shadow_framebuf_offset;
static unsigned long shadow_framebuf_size;

/* Generic utility functions */
int ivtvfb_sleep_timeout(int timeout, int intr)
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

static int ivtv_api_fb_get_framebuffer(struct ivtv *itv, u32 *fbbase, u32 *fblength)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	int rc;

	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_get_framebuffer\n");

	rc = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
			IVTV_API_FB_GET_FRAMEBUFFER, &result, 0, &data[0]);
	*fbbase = data[0];
	*fblength = data[1];
	return rc;
}

static int ivtv_api_fb_get_pixel_format(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_get_pixel_format\n");

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_GET_PIXEL_FORMAT,
			&result, 0, &data[0]);
	return data[0];
}

static int ivtv_api_fb_set_pixel_format(struct ivtv *itv,
		int format)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	data[0] = format;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_set_pixel_format\n");

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
			IVTV_API_FB_SET_PIXEL_FORMAT, &result, 1, &data[0]);
	return result;
}

#if 0
static int ivtv_api_fb_get_status(struct ivtv *itv)
{
    u32 data[IVTV_MBOX_MAX_DATA], result;
    IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_get_status\n");

    ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_GET_STATE,
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
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_get_state\n");

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_GET_STATE,
			&result, 0, &data[0]);
	return data[0];
}

static int ivtv_api_fb_set_state(struct ivtv *itv, int enabled)
{
	u32 params[IVTV_MBOX_MAX_DATA], result;
	params[0] = enabled;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_set_state\n");

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_SET_STATE,
			&result, 1, &params[0]);
	return result;
}

static int ivtv_api_fb_set_framebuffer_window(struct ivtv *itv,
		int left, int top,
		int width, int height)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_set_framebuffer_window\n");
	data[0] = width;
	data[1] = height;
	data[2] = left;
	data[3] = top;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
			IVTV_API_FB_SET_FRAMEBUFFER_WINDOW, &result, 4, &data[0]);
	return result;
}

static int ivtv_api_fb_get_osd_coords(struct ivtv *itv,
		struct ivtv_osd_coords *osd)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_get_osd_coords\n");

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_GET_OSD_COORDS,
			&result, 0, &data[0]);

	osd->offset = data[0] - video_rel_base;
	osd->max_offset = video_width * video_height * 4;
	osd->pixel_stride = data[1];
	osd->lines = data[2];
	osd->x = data[3];
	osd->y = data[4];

	return result;
}

static int ivtv_api_fb_set_osd_coords(struct ivtv *itv,
		const struct ivtv_osd_coords
		*osd)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_set_osd_coords\n");
	data[0] = osd->offset + video_rel_base;
	data[1] = osd->pixel_stride;
	data[2] = osd->lines;
	data[3] = osd->x;
	data[4] = osd->y;

	/* FIXME maybe wait on vsync? */
	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_SET_OSD_COORDS,
			&result, 5, &data[0]);

	return result;
}

static int ivtv_api_fb_get_screen_coords(struct ivtv *itv,
		struct rectangle *r)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_get_screen_coords\n");

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_GET_SCREEN_COORDS,
			&result, 0, &data[0]);

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
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_set_screen_coords\n");
	data[0] = r->x0;
	data[1] = r->y0;
	data[2] = r->x1;
	data[3] = r->y1;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
			IVTV_API_FB_SET_SCREEN_COORDS, &result, 4, &data[0]);
	return result;
}

static int ivtv_api_fb_get_global_alpha(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_get_global_alpha\n");

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_GET_GLOBAL_ALPHA,
			&result, 0, &data[0]);
	return data[1];
}

static int ivtv_api_fb_set_global_alpha(struct ivtv *itv,
		int enable_global,
		int alpha, int enable_local)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_set_global_alpha\n");

	/* Save settings if Firmware reload */
	itv->global_alpha = alpha;
	itv->local_alpha_state = enable_local;
	itv->global_alpha_state = enable_global;

	data[0] = enable_global;
	data[1] = alpha;
	data[2] = !enable_local;
	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
			IVTV_API_FB_SET_GLOBAL_ALPHA, &result, 3, &data[0]);

	return result;
}

static int ivtv_api_fb_get_flicker_state(struct ivtv *itv)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_get_flicker_state\n");

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_GET_FLICKER_STATE,
			&result, 0, &data[0]);
	return data[0];
}

static int ivtv_api_fb_set_flicker_state(struct ivtv *itv,
		int enabled)
{
	u32 params[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_set_flicker_state\n");
	params[0] = enabled;

	ivtv_api(itv, itv->dec_mbox, &itv->dec_msem,
			IVTV_API_FB_SET_FLICKER_STATE, &result, 1, &params[0]);
	return result;
}

/* currently unused */
static int ivtv_api_fb_blt_fill(struct ivtv *itv, int rasterop,
				       int alpha_mode, int alpha_mask_mode,
				       int width, int height, int destmask,
				       u32 destaddr, int deststride,
				       u32 value)
{
    u32 data[IVTV_MBOX_MAX_DATA], result;
    IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_blt_fill\n");
    data[0] = rasterop;
    data[1] = alpha_mode;
    data[2] = alpha_mask_mode;
    data[3] = width;
    data[4] = height;
    data[5] = destmask;
    data[6] = destaddr;
    data[7] = deststride;
    data[8] = value;

    result = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_BLT_FILL, &result,
		      9, &data[0]);
    return result;
}

static int ivtv_api_fb_blt_copy(struct ivtv *itv, int rasterop,
				       int alpha_mode, int alpha_mask_mode,
				       int width, int height, int destmask,
				       u32 destaddr, int deststride,
				       int sourcestride, int sourceaddr)
{
	u32 data[IVTV_MBOX_MAX_DATA], result;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_api_fb_blt_copy: width = %d, height = %d, destaddr = %d, deststride = %d, sourcestride = %d, sourceaddr = %d\n",
			width, height, destaddr, deststride, sourcestride, sourceaddr);

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

	result = ivtv_api(itv, itv->dec_mbox, &itv->dec_msem, IVTV_API_FB_BLT_COPY, &result,
			  10, &data[0]);
	return result;
}


MODULE_PARM(ivtv_fb_card_id, "i");
MODULE_PARM_DESC(ivtv_fb_card_id,
		 "ID number of ivtv card to use as framebuffer device (0-7)");

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
#ifndef LINUX26
	{0, 0, 0, 0, 0, 0}
#else
	0,
	{0, 0, 0, 0, 0}
#endif
};

static struct fb_info fb_info;
#ifndef LINUX26
static struct display disp;
static union {
	u32 cfb32[16];
} fbcon_cmap;
static struct display_switch ivtvfb_sw;
#endif

#ifdef CONFIG_MTRR
static int mtrr = 1;	//++MTY
static unsigned long fb_start_aligned_physaddr;	/* video_base rounded down as required by hardware MTRRs */
static unsigned long fb_end_aligned_physaddr;	/* video_base rounded up as required by hardware MTRRs */
#endif


/* --------------------------------------------------------------------- */
static int _ivtvfb_set_var(struct fb_var_screeninfo *var)
{
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "_ivtvfb_set_var\n");

	if (var->xres != ivtvfb_defined.xres ||
			var->yres != ivtvfb_defined.yres ||
			var->xres_virtual != ivtvfb_defined.xres_virtual ||
			var->yres_virtual > video_height_virtual ||
			var->yres_virtual < video_height ||
			var->xoffset ||
			var->bits_per_pixel != ivtvfb_defined.bits_per_pixel ||
			var->nonstd) {
		return -EINVAL;
	}
	return 0;

}
static int _ivtvfb_get_fix(struct fb_fix_screeninfo *fix)
{
	memset(fix, 0, sizeof(struct fb_fix_screeninfo));
	strcpy(fix->id, "iTVC15 TV out");
	fix->smem_start = video_base;
	fix->smem_len = video_width * video_width * 4;
	fix->type = FB_TYPE_PACKED_PIXELS;
	fix->visual = FB_VISUAL_TRUECOLOR;
	fix->xpanstep = 0;
	fix->ypanstep = 0;
	fix->ywrapstep = 0;
	fix->line_length = video_linelength;
	return 0;
}

#ifndef LINUX26
static int ivtvfb_update_var(int con, struct fb_info *info)
{
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_update_var\n");
	return 0;
}

static int ivtvfb_get_fix(struct fb_fix_screeninfo *fix, int con,
		struct fb_info *info)
{
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_get_fix\n");
	return _ivtvfb_get_fix(fix);
}

static int ivtvfb_get_var(struct fb_var_screeninfo *var, int con,
		struct fb_info *info)
{
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_get_var\n");
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

	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_set_disp\n");
	if (con >= 0)
		display = &fb_display[con];
	else
		display = &disp;	/* used during initialization */

	ivtvfb_get_fix(&fix, con, 0);

	memset(display, 0, sizeof(struct display));
	display->screen_base = video_vbase;
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
	int first=0;
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_set_var\n");
	if (con >= 0) first = 1;

	if (_ivtvfb_set_var(var)){	    
		if (first) {
			IVTV_OSD_KERN_ERR("changing the video mode is not supported\n");
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
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_get_cmap\n");
	return 0;
}

static int ivtvfb_set_cmap(struct fb_cmap *cmap, int kspc, int con,
		struct fb_info *info)
{
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_set_cmap\n");
	return 0;
}

#else
static int ivtvfb_check_var (struct fb_var_screeninfo *var,
		struct fb_info *info)
{
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_check_var\n");
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_set_var\n");
	return (_ivtvfb_set_var(&info->var));
}
static int ivtvfb_set_par(struct fb_info *info){
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtvfb_set_var\n");
	return (_ivtvfb_set_var(&info->var));
}
static int ivtvfb_setcolreg(unsigned regno, unsigned red, unsigned green,
		unsigned blue, unsigned transp, struct fb_info *info)
{
	return (0);
}

static 	void ivtv_fb_fillrect (struct fb_info *info, const struct fb_fillrect *rect)
{
  IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_fillrect\n");
}
static	void ivtv_fb_copyarea (struct fb_info *info, const struct fb_copyarea *region)
{
  IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_copyarea\n");
}
static	void ivtv_fb_imageblit (struct fb_info *info, const struct fb_image *image)
{
  IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_imageblit\n");
}
static	int ivtv_fb_cursor (struct fb_info *info, struct fb_cursor *cursor)
{
  IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_cursor\n");
  return 0;
}
static  void ivtv_fb_rotate(struct fb_info *info, int angle)
{
  IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_rotate\n");
}
static 	int ivtv_fb_sync(struct fb_info *info)
{
  IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_sync\n");
  return 0;
}

#endif

static int ivtv_fb_blt_copy(struct ivtv *itv, int x, int y, int width,
		int height, int source_offset,
		int source_stride)
{
	int rc;
	unsigned long destaddr = ((y * video_width) + x) * 4;

	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_blt_copy \n");
	source_offset += shadow_framebuf_offset;

	rc = ivtv_api_fb_blt_copy(itv, 0xa, 0x1, 0x0, width, height,
			0xffffffff, destaddr, video_width,
			source_stride, source_offset);
	return rc;
}

static int ivtv_fb_blt_fill(struct ivtv *itv, struct ivtvfb_ioctl_blt_fill_args *args)
{
	int rc;
	unsigned long destaddr = IVTV_DEC_MEM_START + video_rel_base + (((args->y * video_width) + args->x) * 4);

	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_blt_fill op %d mode %d mask %d width %d hight %d\n",
		      args->rasterop, args->alpha_mode, args->alpha_mask, args->width, args->height );

	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_blt_fill destMask %x, destAddr %lx stride %d color %x\n",
		       args->destPixelMask, destaddr, video_width, args->colour);

	rc = ivtv_api_fb_blt_fill(itv, args->rasterop, args->alpha_mode, args->alpha_mask, args->width, args->height,
				  args->destPixelMask, destaddr, video_width, args->colour);
	return rc;
}

int ivtvfb_prep_dec_dma_to_device(struct ivtv *itv, unsigned long ivtv_dest_addr,
	char *userbuf, int size_in_bytes)
{
	struct ivtv_stream *stream=
		&itv->streams[IVTV_DEC_STREAM_TYPE_MPG];
	u32 data[IVTV_MBOX_MAX_DATA];
	int ret = 0;
	u32 then;
	int redo_dma = 0;
	unsigned long flags;

	/* Map User DMA */
	if (ivtv_prep_user_dma(itv, 
		&stream->udma, ivtv_dest_addr, userbuf, size_in_bytes) <= 0) {
		IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, 
			"ivtvfb_prep_dec_dma_to_device, "
			"Error with get_user_pages: %d bytes, %d pages returned\n", 
			size_in_bytes, stream->udma.page_count);

		/* get_user_pages must have failed completely */
		return -EIO;	
	}	

	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, 
		"ivtvfb_prep_dec_dma_to_device, %d bytes, %d pages\n", 
		size_in_bytes, stream->udma.page_count);

        /* Decoder DMA State */
        itv->dec_dma_stat.last_xfer = stream->udma.sg_size;
        atomic_set(&itv->dec_dma_stat.type, 1);

	data[0] = stream->udma.SG_handle;
	data[1] = stream->udma.sg_size;
	data[2] = 0x01;

	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO,
		"Sched OSD dma: addr: 0x%08x, array_size 0x%08x, type 0x%08x\n",
		data[0], data[1], data[2]);

/* This uses the cards DMA registers instead of the firmware API
   to send the Decoder DMA, I suspect this is safer than how the
   firmware does it, just because the firmware does it so badly :-) */
	/* Lock DEC DMA */
	down(&itv->DMA_lock);

        /* wait for DMA complete status */
        then = jiffies;
        while (!(ivtv_read_reg(itv->reg_mem +
                        IVTV_REG_DMASTATUS) & 0x00000001))
        {
                ivtvfb_sleep_timeout(HZ/100, 1);
                if ((jiffies - then) > (HZ*3)) {
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
				"OSD: REG_DMASTATUS wait failed\n");	

			ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                        break;
		}
        }

redo_dma:

        /* put SG Handle into register */
	spin_lock_irqsave(&itv->DMA_slock, flags);
        ivtv_write_reg(data[0], itv->reg_mem + IVTV_REG_DECDMAADDR);

        /* Send DMA */
        ivtv_write_reg(0x01, itv->reg_mem + IVTV_REG_DMAXFER);
	spin_unlock_irqrestore(&itv->DMA_slock, flags);

        /* wait for last DMA to be finished */
	then = jiffies;
        while (!(ivtv_read_reg(itv->reg_mem + IVTV_REG_DECSG1LEN) & 0x80000000))
        {
		/* DMA Error */
		if (ivtv_read_reg(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x00000014)
			break;

                ivtvfb_sleep_timeout(HZ/100, 1);
                if ((jiffies - then) > HZ) {
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
				"OSD: REG_DECSG1LEN wait failed\n");	

			ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                	/* Reset DMA, cancel last DMA? */
			spin_lock_irqsave(&itv->DMA_slock, flags);
                	ivtv_write_reg(0x00, itv->reg_mem + IVTV_REG_DMAXFER);
			spin_unlock_irqrestore(&itv->DMA_slock, flags);
                        break;
		}
        }

        /* wait for DMA xfer to be done */
        then = jiffies;
        while ((ivtv_read_reg(itv->reg_mem +
                        IVTV_REG_DMAXFER) & 0x00000001))
        {
		/* DMA Error */
		if (ivtv_read_reg(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x00000014)
			break;

                ivtvfb_sleep_timeout(HZ/100, 1);
                if ((jiffies - then) > (HZ*3)) {
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
				"OSD: REG_DMAXFER wait failed\n");	

			ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                        break;
		}
        }

        /* wait for DMA complete status */
        then = jiffies;
        while (!(ivtv_read_reg(itv->reg_mem +
                        IVTV_REG_DMASTATUS) & 0x00000001))
        {
		/* DMA Error */
		if (ivtv_read_reg(itv->reg_mem + IVTV_REG_DMASTATUS) & 0x00000014)
			break;

                ivtvfb_sleep_timeout(HZ/100, 1);
                if ((jiffies - then) > (HZ*3)) {
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
				"OSD: REG_DMASTATUS2 wait failed\n");	

			ivtv_dma_show_status(itv, IVTV_DEBUG_ERR);

                        break;
		}
        }

        /* DMA Error */
        if ((ivtv_read_reg(itv->reg_mem +
                IVTV_REG_DMASTATUS) & 0x00000014))
        {
                IVTV_DEBUG_OSD(IVTV_DEBUG_DMA, "OSD: (%d) DMA Error 0x%08x\n",
                        redo_dma, ivtv_read_reg(itv->reg_mem +IVTV_REG_DMASTATUS));

                /* Reset DMA Error, cancel last DMA? */
		spin_lock_irqsave(&itv->DMA_slock, flags);
                ivtv_write_reg(0x00, itv->reg_mem + IVTV_REG_DMAXFER);
		spin_unlock_irqrestore(&itv->DMA_slock, flags);
                if (redo_dma < 3) {
                        redo_dma++;
                        goto redo_dma;
                } else
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
				"OSD: REDO DMA took too many tries.\n");	
        }

        /* Unmap Last DMA Xfer */
        if (ivtv_unmap_user_dma(itv, &stream->udma) == 1) 
	{
                IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, "Error: unmap osd dma\n");
        }

	/* Lock DEC DMA */
	up(&itv->DMA_lock);

	/* Unlock stream */
        clear_bit(DMA_IN_USE, &stream->udma.u_flags);
        clear_bit(IVTV_F_S_DMAP, &stream->s_flags);

	/* Wake up next OSD/DEC DMA in wait */
	wake_up(&stream->udma.waitq);

	return ret;
}

static int ivtv_fb_prep_frame(struct ivtv *itv,
				     unsigned long destaddr, void *srcaddr,
				     int count)
{
	int rc = 0;
	struct ivtv_stream *stream = /* Use Decoder Stream for locking */
		&itv->streams[IVTV_DEC_STREAM_TYPE_MPG];
	u32 then;
	DECLARE_WAITQUEUE(wait, current);


	/* If needing to re-setup the OSD */
	if (test_and_clear_bit(OSD_RESET_NEEDED, &itv->r_flags)) {
		ivtvfb_setup();
		clear_bit(DMA_IN_USE, &stream->udma.u_flags);
	}

	then = jiffies;
	add_wait_queue(&stream->udma.waitq, &wait);
	do {
		set_current_state(TASK_INTERRUPTIBLE);

		/* Lock Decoder */
		if (/*!(ivtv_read_reg(itv->dec_mem + IVTV_REG_DEC_READY) & 4) &&
			!(ivtv_read_reg(itv->dec_mem + IVTV_REG_DEC_NEEDED)) &&*/
			(ivtv_read_reg((unsigned char *)itv->reg_mem +IVTV_REG_DMASTATUS) & 0x01) &&
			! (ivtv_read_reg((unsigned char *)itv->reg_mem +IVTV_REG_DMASTATUS) & 0x14) &&
			!test_bit(DMA_IN_USE, &stream->udma.u_flags) &&
			!test_and_set_bit(IVTV_F_S_DMAP, &stream->s_flags))
		{
			if (!test_and_set_bit(DMA_IN_USE, &stream->udma.u_flags))
				break;
			else 
				clear_bit(IVTV_F_S_DMAP, &stream->s_flags);	
		}
		IVTV_DEBUG(IVTV_DEBUG_DEC,
                                "OSD: DMA Reg Status: 0x%08x needs 0x%08x has 0x%08x.\n",
				ivtv_read_reg(itv->dec_mem + IVTV_REG_DEC_READY),
				ivtv_read_reg(itv->dec_mem + IVTV_REG_DEC_NEEDED),
				ivtv_read_reg(itv->dec_mem + IVTV_REG_DEC_FILL));

                if (ivtvfb_sleep_timeout(HZ/100, 1)) {
                        /*IVTV_DEBUG(IVTV_DEBUG_ERR,
                                "OSD: User stopped waitq.\n");

                        rc = -EINTR;
                        break;*/
                }

	} while (!rc);
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&stream->udma.waitq, &wait);

	if (rc) /* Myth breaks when not returning the errors, misses things*/
		return rc; 

	/* OSD Address to send DMA to */
	destaddr = IVTV_DEC_MEM_START + video_rel_base + destaddr;

	/* Fill Buffers */
	if (0 != (rc = ivtvfb_prep_dec_dma_to_device(itv, 
					destaddr, (char *) srcaddr, count)))
	{
		/* Clear DMA Pending Flag */
		clear_bit(DMA_IN_USE, &stream->udma.u_flags);
		clear_bit(IVTV_F_S_DMAP, &stream->s_flags);
		wake_up(&stream->udma.waitq);

		IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, 
			"Error: prep osd dma to device rc=%d\n",rc);
	}

	return rc;
}

#ifndef LINUX26
int ivtv_fb_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		  unsigned long arg, int con, struct fb_info *info)
#else
int ivtv_fb_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		  unsigned long arg, struct fb_info *info)
#endif
{

    int rc;

    IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "ivtv_fb_ioctl\n");
    switch (cmd) {
#ifndef LINUX26
    case 0x7777:{
	    while (MOD_IN_USE) MOD_DEC_USE_COUNT;
	    MOD_INC_USE_COUNT;
	    return 0;
    }
#endif
    case IVTVFB_IOCTL_GET_STATE:{
	    struct ivtvfb_ioctl_state_info state;
	    state.status = (ivtv_api_fb_get_state(itv) & 0x7);
	    state.status |= (ivtv_api_fb_get_flicker_state(itv) << 3);
	    state.status |= IVTVFB_STATUS_ENABLED;
	    state.alpha = ivtv_api_fb_get_global_alpha(itv);
	    IVTV_DEBUG_OSD(IVTV_DEBUG_IOCTL, 
		 "IVTVFB_IOCTL_GET_STATE: status = %lu, alpha = %lu\n",
		 state.status, state.alpha);
	    if (copy_to_user((void *) arg, &state, sizeof(state)))
		return -EFAULT;
	    return 0;
	}
    case IVTVFB_IOCTL_SET_STATE:{
	    struct ivtvfb_ioctl_state_info state;
	    if (copy_from_user(&state, (void *) arg, sizeof(state)))
		return -EFAULT;
	    IVTV_DEBUG_OSD(IVTV_DEBUG_IOCTL, "IVTVFB_IOCTL_SET_STATE: status = %lu, alpha = %lu\n",
		 state.status, state.alpha);
	    /* FIXME: the OSD seems to turn itself off, so we enable it no
               matter what the current state is, something odd going on there. */ 
	    ivtv_api_fb_set_state(itv,
			  (state.status & IVTVFB_STATUS_ENABLED) ? 1 : 1);
	    ivtv_api_fb_set_global_alpha(itv,
					 (state.
					  status &
					  IVTVFB_STATUS_GLOBAL_ALPHA) ? 1 :
					 0, state.alpha,
					 (state.
					  status &
					  IVTVFB_STATUS_LOCAL_ALPHA) ? 1 :
					 0);
	    ivtv_api_fb_set_flicker_state(itv,
					  (state.
					   status &
					   IVTVFB_STATUS_FLICKER_REDUCTION)
					  ? 1 : 0);
	    IVTV_DEBUG_OSD(IVTV_DEBUG_IOCTL, "new state = %d\n",
		   ivtv_api_fb_get_state(itv));
	    IVTV_DEBUG_OSD(IVTV_DEBUG_IOCTL, "global alpha now = %d\n",
		   ivtv_api_fb_get_global_alpha(itv));
	    return 0;
	}
    case IVTVFB_IOCTL_PREP_FRAME:{
	    struct ivtvfb_ioctl_dma_host_to_ivtv_args args;
 	    int ret = 0;	
	    /*int data_left, data_pos;*/

	    if (copy_from_user(&args, (void *) arg, sizeof(args)))
		return -EFAULT;

 	    /* Check Count */
	    if (args.count < IVTV_DMA_OSD_ALIGN_XFER) {
                IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"OSD: small xfer offset 0x%08lx source 0x%08lx size %d\n", 
			args.dest_offset, (unsigned long)args.source, args.count);
		return 0;
	    }

	    /* Align */
	    if (args.count%IVTV_DMA_OSD_ALIGN_XFER)	
	    	args.count = args.count - (args.count%IVTV_DMA_OSD_ALIGN_XFER);	

            if (args.count <= 0) {
                IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
                        "OSD: Size is %d!!!\n", args.count);
                return 0; /* Nothing to do */
            }

            /* Bad Offset */
            if (args.dest_offset < 0)
                args.dest_offset = 0;
            else if (args.dest_offset >= video_size) {
                IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
                        "OSD: Offset %ld is greater than buffer!!!\n",
                        args.dest_offset);
                if (video_size > args.count)
                        args.dest_offset = video_size - args.count;
                if (args.dest_offset < 0)
                        args.dest_offset = 0;
                else if (args.dest_offset >= video_size) {
                        IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
                                "OSD: Count %d Offset %ld "
                                "is greater than buffer\n",
                                args.count, args.dest_offset);
                        return 0;
                }
            }

            /* Check Total FB Size */
            if (((args.dest_offset + args.count) > video_size) ||
                        (args.count > video_size))
            {
                IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
                        "OSD: Size is overflowing the framebuffer %ld, "
                        "only %d available\n",
                        (args.dest_offset + args.count), video_size);

                return -E2BIG;
            }

 	    /* Check Source */
	    if (verify_area(VERIFY_READ, (args.source+args.dest_offset), args.count)) {
                IVTV_DEBUG(IVTV_DEBUG_ERR,
			"OSD: Area not verified!!! 0x%08lx\n",
			(unsigned long)args.source);

                IVTV_DEBUG(IVTV_DEBUG_ERR, 
			"OSD: Verify Area failed for offset 0x%08lx source 0x%08lx count %d\n", 
			args.dest_offset, (unsigned long)args.source, args.count);
		return -EINVAL;
	    }

 	    /* Send DMA */ 
            ret = ivtv_fb_prep_frame(itv, (args.dest_offset),
		      (args.source), args.count);

	    /* Failed */ 	
	    if (ret) {
                if (ret != -ERESTARTSYS) {
                   IVTV_DEBUG(IVTV_DEBUG_ERR,
                            "OSD: DMA xfer from 0x%08x of "
                            "%d bytes "
                            "failed with (%d) offset = 0x%08lx, total %d\n",
                            (u32)args.source, args.count, ret,
                            args.dest_offset,
                            (args.count + (unsigned int)args.dest_offset));
                } else {
                   IVTV_DEBUG(IVTV_DEBUG_INFO,
                              "OSD: DMA xfer interrupted so restarting\n");
                }
            } 

	    return ret;
	}
    case IVTVFB_IOCTL_BLT_COPY:{
	    struct ivtvfb_ioctl_blt_copy_args args;
	    if (copy_from_user(&args, (void *) arg, sizeof(args)))
		return -EFAULT;

	    return ivtv_fb_blt_copy(itv, args.x, args.y, args.width,
				    args.height, args.source_stride,
				    args.source_offset);
	}
    case IVTVFB_IOCTL_BLT_FILL:{
	    struct ivtvfb_ioctl_blt_fill_args args;
	    if (copy_from_user(&args, (void *) arg, sizeof(args)))
		return -EFAULT;

	    return ivtv_fb_blt_fill(itv, &args);
	}
    case IVTVFB_IOCTL_GET_ACTIVE_BUFFER:{
	    struct ivtv_osd_coords bufinfo;
	    rc = ivtv_api_fb_get_osd_coords(itv, &bufinfo);
	    return copy_to_user((void *) arg, &bufinfo, sizeof(bufinfo));
	}
    case IVTVFB_IOCTL_SET_ACTIVE_BUFFER:{
	    struct ivtv_osd_coords bufinfo;
	    if (copy_from_user(&bufinfo, (void *) arg, sizeof(bufinfo)))
		return -EFAULT;
	    return ivtv_api_fb_set_osd_coords(itv, &bufinfo);
	}
    case IVTVFB_IOCTL_GET_FRAME_BUFFER:{
	    struct ivtvfb_ioctl_get_frame_buffer getfb;
	    getfb.mem  = (void *)video_vbase;
	    getfb.size =  video_size;
	    getfb.sizex =  video_width;
	    getfb.sizey =  video_height;

	    return copy_to_user((void *) arg, &getfb, sizeof(getfb));
	}
    default:
	return -EINVAL;
    }
    return 0;
}

static ssize_t
ivtv_fb_write(struct file *file, const char *ubuf, size_t count, loff_t *ppos)
{
        unsigned long p = *ppos;
        int err=0;
        unsigned long flags;

	if (p == 0) 
		IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
			"ERROR: Direct writing is not safe for ivtv usage\n");
	return -EINVAL;

        IVTV_DEBUG_OSD(IVTV_DEBUG_INFO,
                "OSD: count=%zd, size=%d itv->osd.pos = %lu\n",
                count, video_size, (unsigned long)ppos);

        if (p > video_size)
                return -ENOSPC;
        if (count >= video_size)
                count = video_size;
        if (count + p > video_size) {
                count = video_size - p;
                err = -ENOSPC;
        }
        if (count) {
                char *base_addr;

                base_addr = video_vbase;

                spin_lock_irqsave(&itv->DMA_slock, flags);
                memcpy_toio(base_addr+p, (char *)ubuf, count);
                spin_unlock_irqrestore(&itv->DMA_slock, flags);

                *ppos += count;
                err = -EFAULT;
        }
        if (count)
                return count;
        return err;
}

static int          
ivtv_fb_mmap(struct fb_info *info, 
	struct file *file, struct vm_area_struct * vma)
{
	IVTV_DEBUG_OSD(IVTV_DEBUG_ERR,
			"MMAP is not safe for ivtv usage\n");
	return -EINVAL;
}

static struct fb_ops ivtvfb_ops = {
	owner:THIS_MODULE,
#ifndef LINUX26
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

#ifndef LINUX26
static void ivtvfb_blank(int blank, struct fb_info *info)
{
    /* Not supported */
}
#endif 

int ivtvfb_setup(void)
{
	int rc;
	u32 fbbase;
	u32 fblength;
	struct ivtv_osd_coords osd;
	struct rectangle rect;

	rc = ivtv_api_fb_set_pixel_format(itv, 4);      // 4 = AlphaRGB 8:8:8:8

	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "Current pixel format = %d\n",
			ivtv_api_fb_get_pixel_format(itv));

	video_width = 720;
	if (itv->std & V4L2_STD_625_50) {
		video_height = 576;
	} else {
		video_height = 480;
	}

        /* set number of internal decoder buffers */
        ivtv_vapi(itv,
                IVTV_API_DEC_DISPLAY_BUFFERS, 1, itv->dec_options.decbuffers);

	rc = ivtv_api_fb_get_framebuffer(itv, &fbbase, &fblength);
	IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, 
			"Framebuffer is at decoder-relative address 0x%08x and has %d bytes.\n",
			fbbase, fblength);

	rc = ivtv_api_fb_get_osd_coords(itv, &osd);
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, 
			"OSD: offset = 0x%08x (max offset = 0x%08x), pixel_stride = %d, lines = %d, x = %d, y = %d\n",
			(u32)osd.offset, (u32)osd.max_offset, osd.pixel_stride, osd.lines, osd.x,
			osd.y);

	/* setup OSD and screen for PAL */
	if (itv->std & V4L2_STD_625_50) {
		osd.lines = 576;
		rc = ivtv_api_fb_set_osd_coords(itv, &osd);
		if (rc)
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, "failed setting PAL osd\n");

		rect.x0 = 0;
		rect.x1 = 720;
		rect.y0 = 0;
		rect.y1 = 576;
		rc = ivtv_api_fb_set_screen_coords(itv, &rect);
		if (rc)
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, "failed setting PAL screen\n");

		/* Set Window size */
		ivtv_api_fb_set_framebuffer_window(itv, 0, 0, 720, 576);

	} else { /* Set Window size */
		osd.lines = 480;
		rc = ivtv_api_fb_set_osd_coords(itv, &osd);
		if (rc)
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, "failed setting PAL osd\n");

		rect.x0 = 0;
		rect.x1 = 720;
		rect.y0 = 0;
		rect.y1 = 480;
		rc = ivtv_api_fb_set_screen_coords(itv, &rect);
		if (rc)
			IVTV_DEBUG_OSD(IVTV_DEBUG_ERR, "failed setting PAL screen\n");

		/* Set Window size */
		ivtv_api_fb_set_framebuffer_window(itv, 0, 0, 720, 480);
	}

	rc = ivtv_api_fb_get_screen_coords(itv, &rect);
	IVTV_OSD_KERN_INFO("screen coords: [%d %d] -> [%d %d]\n",
			rect.x0, rect.y0, rect.x1, rect.y1);

	IVTV_OSD_KERN_INFO("original global alpha = %d\n",
			ivtv_api_fb_get_global_alpha(itv));

	/*
	 * Normally a 32-bit RGBA framebuffer would be fine, however XFree86's fbdev
	 * driver doesn't understand the concept of alpha channel and always sets
	 * bits 24-31 to zero when using a 24bpp-on-32bpp framebuffer device. We fix
	 * this behavior by enabling the iTVC15's global alpha feature, which causes     * the chip to ignore the per-pixel alpha data and instead use one value (e.g.,
	 * full brightness = 255) for the entire framebuffer. The local alpha is also
	 * disabled in this step.
	 *
	 *++MTY Need to update http://ivtv.sourceforge.net/ivtv/firmware-api.html
	 *      call 0x4b: param[2] says 1 = enable local alpha, when in reality
	 *      it means *disable* local alpha...
	 *
	 */
	rc = ivtv_api_fb_set_state(itv, 1);     // 1 = enabled
	IVTV_OSD_KERN_INFO("current OSD state = %d\n",
			ivtv_api_fb_get_state(itv));

	ivtv_api_fb_set_global_alpha(itv, 
			itv->global_alpha_state, 
			itv->global_alpha, 
			itv->local_alpha_state);

	IVTV_OSD_KERN_INFO("new global alpha = %d (%d %d %d)\n",
			ivtv_api_fb_get_global_alpha(itv),
			itv->global_alpha_state, itv->global_alpha,
			itv->local_alpha_state);

	video_rel_base = fbbase;
	video_base = itv->base_addr + IVTV_DECODER_OFFSET + video_rel_base;
	video_linelength = 4 * osd.pixel_stride;

	/* If set then framebuffer is small, else about 2 worth */
	video_size = video_width * video_height * 4;	

	/* Make sure it really fits */
	if (video_size > fblength) {
		IVTV_OSD_KERN_ERR("ERROR: OSD size = 0x%08x too big, fb = 0x%08x\n",
			video_size, fblength);
		video_size = fblength;
	}

	shadow_framebuf_size = video_size;
	shadow_framebuf_offset = (video_size - shadow_framebuf_size) & ~3;

	return 0;
}

int __init ivtvfb_init(void)
{
#ifdef LINUX26
	struct fb_fix_screeninfo fix;
#endif
	int i;	

	if ((ivtv_fb_card_id < 0) || (ivtv_fb_card_id >= ivtv_cards_active)) {
		IVTV_OSD_KERN_ERR(
				"ivtv_fb_card_id parameter is out of range (valid range: 0-%d)\n",
				ivtv_cards_active - 1);
		return -1;
	}


        /* find first tvout enabled card if none is specified */
        if (ivtv_fb_card_id == 0)
        {
                i = 0;
                while (i < ivtv_cards_active)
                {
                        itv = &ivtv_cards[i];
                        if (itv && (itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT))
                        {
                                ivtv_fb_card_id = i;
                                break;
                        }
                        ++i;
                }
        }

	itv = &ivtv_cards[ivtv_fb_card_id];
	if (!itv || !(itv->v4l2_cap & V4L2_CAP_VIDEO_OUTPUT)) {
		IVTV_OSD_KERN_ERR(
				"Specified card (id %d) is either not present or does not support TV out\n",
				ivtv_fb_card_id);
		return -1;
	}

	IVTV_OSD_KERN_INFO(
			"Framebuffer module loaded (attached to ivtv card id %d)\n",
			ivtv_fb_card_id);

	/* Setup OSD */
	ivtvfb_setup();

	video_vbase = itv->dec_mem + video_rel_base;
	if (!video_vbase) {
		IVTV_OSD_KERN_ERR(
			"abort, video memory 0x%x @ 0x%lx isn't mapped!!!\n",
			video_size, video_base);
		return -EIO;
	}
	IVTV_OSD_KERN_INFO(
			"framebuffer at 0x%lx, mapped to 0x%p, size %dk\n",
			video_base, video_vbase, video_size / 1024);
	IVTV_OSD_KERN_INFO("mode is %dx%dx%d, linelength=%d\n",
			video_width, video_height, 32, video_linelength);

	ivtvfb_defined.xres = video_width;
	ivtvfb_defined.yres = video_height;
	ivtvfb_defined.xres_virtual = video_width;
	ivtvfb_defined.yres_virtual = video_height;
	ivtvfb_defined.bits_per_pixel = 32;
	video_height_virtual = ivtvfb_defined.yres_virtual;

	/* some dummy values for timing to make fbset happy */
	ivtvfb_defined.pixclock = 10000000 / video_width * 1000 / video_height;
	ivtvfb_defined.left_margin = (video_width / 8) & 0xf8;
	ivtvfb_defined.right_margin = 32;
	ivtvfb_defined.upper_margin = 16;
	ivtvfb_defined.lower_margin = 4;
	ivtvfb_defined.hsync_len = (video_width / 8) & 0xf8;
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
		while (!(video_size & (1 << size_shift))) {
			size_shift--;
		}
		size_shift++;

		fb_start_aligned_physaddr = video_base & ~((1 << size_shift) - 1);
		fb_end_aligned_physaddr =
			(video_base + (1 << size_shift) - 1) & ~((1 << size_shift) -
								 1);
		if (mtrr_add
				(fb_start_aligned_physaddr,
				 (fb_end_aligned_physaddr - fb_start_aligned_physaddr),
				 MTRR_TYPE_WRCOMB, 1) < 0) {
			IVTV_OSD_KERN_ERR(
					"warning: mtrr_add() failed to add write combining region 0x%08x-0x%08x\n",
					(unsigned int) fb_start_aligned_physaddr,
					(unsigned int) fb_end_aligned_physaddr);
		}
	}
#endif

	fb_info.node = -1;
	fb_info.flags = FBINFO_FLAG_DEFAULT;
	fb_info.fbops = &ivtvfb_ops;

#ifndef LINUX26
	strcpy(fb_info.modename, "iTVC15 TV out");
	fb_info.fontname[0] = '\0';
	fb_info.changevar = NULL;
	fb_info.disp = &disp;
	fb_info.switch_con = NULL;	//&ivtvfb_switch; (fbcon will ignore it then)
	fb_info.updatevar = &ivtvfb_update_var;
	fb_info.blank = &ivtvfb_blank;
	ivtvfb_set_disp(-1);
#else
	_ivtvfb_get_fix(&fix);
	fb_info.var = ivtvfb_defined;
	fb_info.fix = fix;
	fb_info.screen_base=video_vbase;
	fb_info.fbops = &ivtvfb_ops;
	fb_alloc_cmap(&fb_info.cmap, 0, 0);
#endif

	if (register_framebuffer(&fb_info) < 0)
		return -EINVAL;

#ifndef LINUX26
	itv->fb_id = GET_FB_IDX(fb_info.node);

	IVTV_OSD_KERN_INFO("fb%d: %s frame buffer device\n",
			itv->fb_id, fb_info.modename);
#else
	itv->fb_id = fb_info.node;

	IVTV_OSD_KERN_INFO("fb%d: %s frame buffer device\n",
			itv->fb_id, fix.id);
#endif

    	/* Allocate DMA */
    	ivtv_alloc_user_dma(itv,
                &itv->streams[IVTV_DEC_STREAM_TYPE_MPG].udma,
                IVTV_DMA_SG_OSD_ENT);

	/* Allow module to unload */
#ifndef LINUX26
	while (MOD_IN_USE) MOD_DEC_USE_COUNT;
	MOD_INC_USE_COUNT;
#endif

	return 0;
}


static void ivtvfb_cleanup(void)
{
	IVTV_DEBUG_OSD(IVTV_DEBUG_INFO, "Unloading framebuffer module\n");
	unregister_framebuffer(&fb_info);
#ifdef CONFIG_MTRR
	mtrr_del(-1, fb_start_aligned_physaddr,
			(fb_end_aligned_physaddr - fb_start_aligned_physaddr));
#endif

    	/* Free DMA */
        ivtv_free_user_dma(itv,
            &itv->streams[IVTV_DEC_STREAM_TYPE_MPG].udma);

	itv->fb_id = -1;
}

module_init(ivtvfb_init);
module_exit(ivtvfb_cleanup);
