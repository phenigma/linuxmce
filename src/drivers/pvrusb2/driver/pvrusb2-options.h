/*
 *
 *  $Id: pvrusb2-options.h 1572 2007-02-28 04:57:33Z isely $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef __PVRUSB2_OPTIONS_H
#define __PVRUSB2_OPTIONS_H

/*

  This header sets up various environmental options for use by the
  standalone driver.  It is edited out for V4L.

*/

#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18))
/* Bring in config.h for older kernels.  For 2.6.18 or later the build
   system does this for us. */
#include <linux/config.h>
#endif

/*
   List of driver configuration directives.  These are intended to be
   automatically set up, but can be overridden manaually here if it
   makes sense for your environment.

     PVR2_ENABLE_LOG_STATUS - When defined, VIDIOC_LOG_STATUS
       functionality is enabled.  This is a good thing to have on, but
       only appeared starting with 2.6.15 kernels.

     PVR2_ENABLE_SAA711X - When defined, this enables compilation of
       code to use the new saa711x module in V4L (which is actually
       saa7115.ko in 2.6.15, **NOT** saa711x.ko in 2.6.15 which
       appears to be an inferior incomplete module that I believe will
       disappear soon).  Normally we want to enable this, but if the
       kernel version is earlier than 2.6.15, then we have to shut
       this off because otherwise the driver will not compile (because
       the module didn't appear in the kernel until 2.6.15).

     PVR2_ENABLE_CX2584X_OLD - When defined, this enables compilation
       of code to use cx25840.ko (for kernels older than 2.6.16),
       which is needed for newer revisions of the PVR USB2 hardware.

     PVR2_ENABLE_CX2584X_V4L - When defined, this enables compilation
       of code to use cx25840.ko (for kernels 2.6.16 or newer), which
       is needed for newer revisions of the PVR USB2 hardware.

     PVR2_ENABLE_WM8775 - When defined, this enables compilation of
       code to use wm8775.ko, which is needed for newer revisions of
       the PVR USB2 hardware.

     PVR2_ENABLE_SAA7115 - When defined, this enables the compilation
       of code to use the saa7115 that comes from ivtv.  Normally we
       always want to enable this (but in v4l-cvs it makes no sense to
       have it around).

     PVR2_EEPROM_INDIRECT - When defined, use indirect method to
       access the device eeprom.  This is technically the cleaner way
       to get at the eeprom data, but unfortunately tveeprom.ko
       doesn't seem to work right when doing this on our behalf.  Thus
       you should probably just never define this.  But if you are
       morbidly curious, you can try it...

     PVR2_EEPROM_IVTV - When defined and only if PVR2_EEPROM_INDIRECT
       is not set (otherwise there is no effect), this causes the
       direct method of tveeprom to be adjusted to be as forgiving as
       possible regarding the version of tveeprom that it is dealing
       with.  This selects the "horrible hack" method referred to in
       the pvrusb2-eeprom.html.  When not defined, we just assume
       we're dealing with whatever tveeprom.ko that V4L has
       implemented and don't make allowances for compatibility
       fracturing brain damage.

     PVR2_ENABLE_TVEEPROM_FAKECLIENT - When defined, use more recent
       function call interface to tveeprom.

     PVR2_ENABLE_TUNER_SETUPADDR - When defined, use more recent V4L2
       API for setting the tuner type.

     PVR2_ENABLE_V4L_PRINT_IOCTL - When defined, we'll use the new
       v4l_print_ioctl() function to print out ioctl info.

     PVR2_ENABLE_NEW_ROUTING - When defined, we assume that the V4L
       environment in the kernel has the routing / input selection
       cleanups, which affects how we operate several modules.  This
       situation will be true when building inside the V4L repo or
       when in a 2.6.17 kernel or later.

     PVR2_ENABLE_CX25840_FWSEND_HACK - When defined, enable special
       code in the I2C adapter to trap and explicitly handle attempts
       to download firmware to the cx25840 where the chunk size is too
       large.  This bit of hackery eliminates the need to patch the
       FWSEND value in the cx25840 module.  Obviously the "right"
       solution in the end is to fix FWSEND, and that has been done
       and committed for kernels 2.6.17 or later (or the V4L
       repository).  However the cx25840 module in the 2.6.16 and
       2.6.15 kernels are problematic and must otherwise be
       hand-patched.  Enabling this option solves that problem.

     PVR2_ENABLE_CX25840_ALGO_HACK - When defined, work around a
       serious bug in cx25840 where that module will stomp on our
       internal state.  The problem is that cx25840 is trying to reach
       through an I2C adapter-private context pointer in an attempt to
       manipulate the driver.  The problem however is that it is
       assuming a specific type of I2C adapter driver which is
       incorrect in the case of pvrusb2.  The cx25840 module really
       has no business trying to touch the "private parts" of another
       driver.  This is being fixed in the main V4L repo, but for
       older kernels (2.6.15 and probably also 2.6.16) we still need
       to do a workaround.  What this will do is pad the front of the
       pvr2_hdw structure so that what cx25840 messes with won't
       actually harm anything.  This is ugly, ugly, ugly.  But you
       gotta do what you gotta do...

     PVR2_ENABLE_V4LCVS - When defined, assume we're building inside
       of the V4L CVS repository.  This is normally off.  Note: If you
       pass V4LDIR=path_to_v4l_tree to make, then this option will be
       turned on (and the value of V4LDIR will be used to adjust the
       include search path so that the v4l tree headers will be found
       and used).  I have this here to help with my own debugging of
       the standalone driver; if you really want to build with the
       latest v4l snapshot, there's a pvrusb2 driver version already
       resident there.  In other words, just build v4l in that case
       and don't bother with this standalone snapshot.

     PVR2_CX2341X_HDR_MEDIA - When defined, look for cx2341x.h in the
       media header directory.

     PVR2_ENABLE_STDADJ - When defined, include logic to infer
       additional video standards beyond that which is reported by
       tveeprom.  This is used to enable standards that we are
       reasonably sure should be available but which are not (yet)
       reported by tveeprom.

     PVR2_ENABLE_PVRUSB2_AUDIO_OLD - When defined, build
       pvrusb2-audio-old.  This is an integration for msp3400 using an
       older API.  You don't need this unless you're building with
       msp3400 from an old kernel or you are trying to play nicely
       with an old version of ivtv.

     PVR2_ENABLE_CX2341XMOD - Use the cx2341x module instead of
       directly operating the cx23416 encoder ourself.  This is a new
       module that should appear in the 2.6.18 kernel.

     PVR2_ENABLE_EXT_CTRLS - Turn on implementation of the new
       VIDIOC_S_EXT_CTRLS ioctl in the driver's ioctl()
       implementation.  This should normally be on for kernels 2.6.18
       or later or when built against a V4L repository.

     PVR2_ENABLE_REGISTER_ACCESS - Turn on code which makes possible
        direct register access in chip-level drivers which support it.

     PVR2_ENABLE_OLDREGISTER_ACCESS - Change register access API and
        implementation to match with V4L code prior to 2.6.22 kernel.

     PVR2_ENABLE_TDA9887 - Enable use of tda9887 module.  With kernel
        version 2.6.18 or later, tda9887 is merged into tuner and we
        no longer need to mess with it any further.  It's otherwise
        harmless to leave it enabled.

     PVR2_ENABLE_V4L2COMMON - Enable use of media/v4l2-common.h where
        appropriate.  Later kernel versions put critical things in
        this newer header.

     PVR2_ENABLE_OLDAUDC - Look for audio macros in old audiochip.h
       instead of v4l2-common.h where it is found in modern kernels...

     PVR2_ENABLE_V4L2DEV - Enable use of media/v4l2-dev.h where
        appropriate.  Later kernel versions put critical things in
        this newer header.

     PVR2_EMULATE_KZALLOC - Environment doesn't support the use of
        kzalloc.

     PVR2_ENABLE_AUTOMODESWITCH - Compile in logic to make possible
        automatic mode switching based on tuned frequency.

   MCI <isely@pobox.com> 26-Nov-2005 - About saa7115 vs saa711x...
   These are two different modules but they serve the same purpose.
   The "saa7115" that we have known all along comes from ivtv, while
   "saa711x" is new and comes from v4l.  Unfortunately their APIs are
   completely different.  The pvrusb2 driver can use either one; the
   results are the same.  If only one choice is enabled here then that
   is all the driver will look for.  If both choices are enabled, then
   the driver will detect which one is loaded and use it.  You *must*
   have one of these modules loaded and operable by this driver in
   order for video processing to function.  Making matters more
   confusing is that within V4L at least for 2.6.15 itself, there are
   two variations of this module, saa711x.ko and saa7115.ko.  The one
   named saa7115.ko *is* actually the saa711x we're talking about
   here.  The one named saa711x.ko is apparently an incomplete dead
   end that AFAIK will be going away soon, whereupon saa7115.ko will
   be renamed on top of it.  Even though it's currently known as
   saa7115.ko, internally it really is saa711x.ko with the entirely
   different API and it identifies itself to outside users as saa711x.
   Short summary: Only load saa7115.ko not saa711x.ko (and that is
   what the driver will attempt for now).  Clear as mud?  Yeah, I
   know.  Very confusing.  This craziness is likely due the ongoing
   merge of ivtv into v4l.  Please don't shoot the messenger :-)


*/

/* Always enable standards hacking */
#define PVR2_ENABLE_STDADJ

/* Always enable the use of the ivtv saa7115 module */
#define PVR2_ENABLE_SAA7115

/* Enable the use of the cx25840 old module if kernel is older than 2.6.16 */
#if (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,15))
#define PVR2_ENABLE_CX2584X_OLD
#endif

/* Enable the use of the cx25840 v4l module for kernel 2.6.16 or later. */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16))
#define PVR2_ENABLE_CX2584X_V4L
#endif

/* For stand-alone builds, be as forgiving as possible with tveeprom */
#define PVR2_EEPROM_IVTV

/* Enable various things if kernel is recent enough or we're in V4L CVS */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22)) \
    || defined(PVR2_ENABLE_V4LCVS)
#else
#define PVR2_ENABLE_OLDREGISTER_ACCESS
#endif

/* Enable various things if kernel is recent enough or we're in V4L CVS */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,17)) \
    || defined(PVR2_ENABLE_V4LCVS)
#define PVR2_ENABLE_NEW_ROUTING
#else
#define PVR2_ENABLE_CX25840_FWSEND_HACK
#define ENABLE_PVRUSB2_AUDIO_OLD
#endif

/* Enable various things if kernel is recent enough or we're in V4L CVS */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)) \
    || defined(PVR2_ENABLE_V4LCVS)
#define PVR2_CX2341X_HDR_MEDIA
#define PVR2_ENABLE_EXT_CTRLS
#define PVR2_ENABLE_CX2341XMOD
#define PVR2_ENABLE_V4L2DEV
#else
#define PVR2_ENABLE_TDA9887
#endif


/* Enable various things if kernel is recent enough or we're in V4L CVS */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,16)) \
    || defined(PVR2_ENABLE_V4LCVS)
#define PVR2_ENABLE_V4L_PRINT_IOCTL
#else
#define PVR2_ENABLE_OLDAUDC
#endif

/* Enable various things if kernel is recent enough or we're in V4L CVS */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,15)) \
    || defined(PVR2_ENABLE_V4LCVS)
#define PVR2_ENABLE_V4L2COMMON
#define PVR2_ENABLE_SAA711X
#define PVR2_ENABLE_LOG_STATUS
#define PVR2_ENABLE_WM8775
#define PVR2_ENABLE_REGISTER_ACCESS
#endif

/* Enable various things if kernel is recent enough or we're in V4L CVS */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,14)) \
    || defined(PVR2_ENABLE_V4LCVS)
#define PVR2_ENABLE_TVEEPROM_FAKECLIENT
#endif

/* Enable various things if kernel is recent enough or we're in V4L CVS */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,13)) \
    || defined(PVR2_ENABLE_V4LCVS)
#define PVR2_ENABLE_TUNER_SETUPADDR
#endif

/* Enable cx25840 algo hack only if we are in 2.6.15 or 2.6.16 AND
   we're not in V4L CVS */
#if ((LINUX_VERSION_CODE == KERNEL_VERSION(2,6,15)) || \
	(LINUX_VERSION_CODE == KERNEL_VERSION(2,6,16))) && \
	!defined (PVR2_ENABLE_V4LCVS)
#define PVR2_ENABLE_CX25840_ALGO_HACK
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16))
#define mutex_unlock(a) up(a)
#define mutex_lock(a) down(a)
#define mutex_init(a) init_MUTEX(a)
#endif

#if ((LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21)) && \
	!defined (PVR2_ENABLE_V4LCVS))
#define VIDIOC_DBG_S_REGISTER VIDIOC_INT_S_REGISTER
#define VIDIOC_DBG_G_REGISTER VIDIOC_INT_G_REGISTER
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14))
#define PVR2_EMULATE_KZALLOC
void *kzalloc(unsigned int,unsigned int);
#endif

#endif /* __PVRUSB2_OPTIONS_H */

/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 70 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
