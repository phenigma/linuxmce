/*
 *
 *  $Id: pvrusb2-sysfs.c,v 1.1 2005/11/14 13:31:24 mchehab Exp $
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

#include <linux/string.h>
#include <linux/slab.h>
#include <asm/semaphore.h>
#include "pvrusb2-sysfs.h"
#include "pvrusb2-hdw.h"
#include "pvrusb2-debug.h"
#include "pvrusb2-debugifc.h"

#define pvr2_sysfs_trace(...) pvr2_trace(PVR2_TRACE_SYSFS,__VA_ARGS__)

static char *item_names[PVR2_CID_COUNT] = {
	[PVR2_CID_BRIGHTNESS] = "ctl_brightness",
	[PVR2_CID_CONTRAST] = "ctl_contrast",
	[PVR2_CID_SATURATION] = "ctl_saturation",
	[PVR2_CID_HUE] = "ctl_hue",
	[PVR2_CID_VOLUME] = "ctl_volume",
	[PVR2_CID_BALANCE] = "ctl_balance",
	[PVR2_CID_BASS] = "ctl_bass",
	[PVR2_CID_TREBLE] = "ctl_treble",
	[PVR2_CID_MUTE] = "ctl_mute",
	[PVR2_CID_SRATE] = "ctl_srate",
	[PVR2_CID_AUDIOBITRATE] = "ctl_audio_bitrate",
	[PVR2_CID_AUDIOCRC] = "ctl_audio_crc",
	[PVR2_CID_AUDIOEMPHASIS] = "ctl_audio_emphasis",
	[PVR2_CID_VBR] = "ctl_vbr",
	[PVR2_CID_AVERAGEVIDEOBITRATE] = "ctl_video_average_bitrate",
	[PVR2_CID_PEAKVIDEOBITRATE] = "ctl_video_peak_bitrate",
	[PVR2_CID_VIDEOSTANDARD] = "ctl_video_standard",
	[PVR2_CID_INPUT] = "ctl_input",
	[PVR2_CID_AUDIOMODE] = "ctl_audio_mode",
	[PVR2_CID_FREQUENCY] = "ctl_frequency",
	[PVR2_CID_HRES] = "ctl_resolution_hor",
	[PVR2_CID_VRES] = "ctl_resolution_ver",
	[PVR2_CID_INTERLACE] = "ctl_interlace",
	[PVR2_CID_AUDIOLAYER] = "ctl_audio_layer",
	[PVR2_CID_CHANNEL] = "ctl_channel",
	[PVR2_CID_CHANPROG_ID] = "ctl_freq_table_channel",
	[PVR2_CID_CHANPROG_FREQ] = "ctl_freq_table_value",
	[PVR2_CID_SIGNAL_PRESENT] = "ctl_signal_present",
	[PVR2_CID_STREAMING_ENABLED] = "ctl_streaming_enabled",
	[PVR2_CID_HSM] = "ctl_usb_speed",
	[PVR2_CID_SUBSYS_MASK] = "ctl_debug_subsys_mask",
	[PVR2_CID_SUBSYS_STREAM_MASK] = "ctl_debug_subsys_stream_mask",
};

struct pvr2_sysfs {
	struct pvr2_channel channel;
	struct class_device *class_dev;
	struct pvr2_sysfs_debugifc *debugifc;
	struct pvr2_sysfs_ctl_item *item_first;
	struct pvr2_sysfs_ctl_item *item_last;
	struct sysfs_ops kops;
	struct kobj_type ktype;
	struct class_device_attribute attr_v4l_minor_number;
	struct class_device_attribute attr_unit_number;
};

struct pvr2_sysfs_debugifc {
	struct class_device_attribute attr_debugcmd;
	struct class_device_attribute attr_debuginfo;
};

struct pvr2_sysfs_ctl_item {
	struct class_device_attribute attr_name;
	struct class_device_attribute attr_min;
	struct class_device_attribute attr_max;
	struct class_device_attribute attr_enum;
	struct class_device_attribute attr_val;
	int attr_id;
	struct pvr2_sysfs *chptr;
	struct pvr2_sysfs_ctl_item *item_next;
	struct attribute *attr_gen[5];
	struct attribute_group grp;
};

struct pvr2_sysfs_class {
	struct class class;
};

static ssize_t show_name(int id,struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	const char *name;

	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	name = pvr2_hdw_get_ctl_name(sfp->channel.hdw,id);

	pvr2_sysfs_trace("pvr2_sysfs(%p) show_name(cid=%d) is %s",sfp,id,name);

	if (!name) return -EINVAL;

	return scnprintf(buf,PAGE_SIZE,"%s\n",name);
}

static ssize_t show_min(int id,struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	int val;

	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	val = pvr2_hdw_get_ctl_min_value(sfp->channel.hdw,id);

	pvr2_sysfs_trace("pvr2_sysfs(%p) show_min(cid=%d) is %d",sfp,id,val);

	return scnprintf(buf,PAGE_SIZE,"%d\n",val);
}

static ssize_t show_max(int id,struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	int val;

	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	val = pvr2_hdw_get_ctl_max_value(sfp->channel.hdw,id);

	pvr2_sysfs_trace("pvr2_sysfs(%p) show_max(cid=%d) is %d",sfp,id,val);

	return scnprintf(buf,PAGE_SIZE,"%d\n",val);
}

static ssize_t show_val_int(int id,struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	int val;

	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	val = pvr2_hdw_get_ctl_value(sfp->channel.hdw,id);

	pvr2_sysfs_trace("pvr2_sysfs(%p) show_val_int(cid=%d) is %d",
			 sfp,id,val);

	return scnprintf(buf,PAGE_SIZE,"%d\n",val);
}

static ssize_t show_val_enum(int id,struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	int val;
	const char *name;

	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	val = pvr2_hdw_get_ctl_value(sfp->channel.hdw,id);

	name = pvr2_hdw_get_ctl_value_name(sfp->channel.hdw,id,val);

	pvr2_sysfs_trace("pvr2_sysfs(%p) show_val_enum(cid=%d) is %s (%d)",
			 sfp,id,name,val);

	return scnprintf(buf,PAGE_SIZE,"%s\n",name);
}

static ssize_t show_enum(int id,struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	int minval,maxval,val;
	const char *name;
	ssize_t cnt = 0;

	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	minval = pvr2_hdw_get_ctl_min_value(sfp->channel.hdw,id);
	maxval = pvr2_hdw_get_ctl_max_value(sfp->channel.hdw,id);
	for (val = minval; val <= maxval; val++) {
		name = pvr2_hdw_get_ctl_value_name(sfp->channel.hdw,id,val);
		cnt += scnprintf(buf+cnt,PAGE_SIZE-cnt,"%s\n",name);
	}
	pvr2_sysfs_trace("pvr2_sysfs(%p) show_enum(cid=%d)",sfp,id);
	return cnt;
}

static int store_val_any(int id,struct pvr2_sysfs *sfp,
                         const char *buf,unsigned int count)
{
	int val,minval,maxval;
	int ch,ret;
	const char *nv;
	unsigned int nl;
	int negfl;

	/* Trim leading / trailing whitespace */
	while (count) {
		ch = buf[0];
		if ((ch > 32) && (ch < 127)) break;
		buf++;
		count--;
	}
	while (count) {
		ch = buf[count-1];
		if ((ch > 32) && (ch < 127)) break;
		count--;
	}

	/* Is this an enum?  Look for a string value */
	minval = pvr2_hdw_get_ctl_min_value(sfp->channel.hdw,id);
	maxval = pvr2_hdw_get_ctl_max_value(sfp->channel.hdw,id);
	for (val = minval; val <= maxval; val++) {
		nv = pvr2_hdw_get_ctl_value_name(sfp->channel.hdw,id,val);
		if ((!nv) && (val == minval)) break; /* Not an enum */
		pvr2_sysfs_trace("pvr2_sysfs(%p) trying ctl_id %d val %d",
				 sfp,id,val);
		if (!nv) {
			pvr2_sysfs_trace("pvr2_sysfs(%p) no pointer",sfp);
			continue;
		}
		nl = strlen(nv);
		if (nl != count) {
			pvr2_sysfs_trace("pvr2_sysfs(%p) count mismatch"
					 " %d != %d",
					 sfp,count,nl);
			continue;
		}
		if (memcmp(buf,nv,nl)) {
			pvr2_sysfs_trace(
				"pvr2_sysfs(%p) name mismatch"
				" >>%.*s<< != >>%.*s<<",
				sfp,nl,buf,nl,nv);
			continue;
		}
		pvr2_sysfs_trace("pvr2_sysfs(%p) store_val_any(cid=%d)"
				 " is enum %s",
				 sfp,id,nv);
		ret = pvr2_hdw_set_ctl_value(sfp->channel.hdw,id,val);
		pvr2_hdw_commit_ctl(sfp->channel.hdw);
		return 0;
	}
	if (val > minval) {
		pvr2_sysfs_trace(
			"pvr2_sysfs(%p) store_val_any(cid=%d)"
			" unmatched enum >>%.*s<<",
			sfp,id,count,buf);
	}

	/* Try to parse as a number */
	negfl = 0;
	val = 0;
	if (count) {
		if (buf[0] == '-') {
			negfl = !0;
			buf++;
			count--;
		} else if (buf[0] == '+') {
			buf++;
			count--;
		}
	}
	while (count) {
		ch = buf[0];
		if ((ch < '0') || (ch > '9')) break;
		val = val * 10;
		val += (ch - '0');
		buf++;
		count--;
	}
	if (!count) {
		if (negfl) val = -val;
		pvr2_sysfs_trace("pvr2_sysfs(%p) store_val_any(cid=%d)"
				 " int is %d",
				 sfp,id,val);

		ret = pvr2_hdw_set_ctl_value(sfp->channel.hdw,id,val);
		pvr2_hdw_commit_ctl(sfp->channel.hdw);
		return ret;
	}

	return -EINVAL;
}

static int store_val_multi(int id,struct pvr2_sysfs *sfp,
                           const char *buf,unsigned int count)
{
	unsigned int count2;
	int ret;

	while (count) {
		count2 = 0;
		while ((count2 < count) && (buf[count2] != '\n')) count2++;
		ret = store_val_any(id,sfp,buf,count2);
		if (ret < 0) return ret;
		if (count2 < count) count2++;
		buf += count2;
		count -= count2;
	}
	return 0;
}

static ssize_t store_val_int(int id,struct class_device *class_dev,
                             const char *buf,size_t count)
{
	struct pvr2_sysfs *sfp;
	int ret;
	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	ret = store_val_multi(id,sfp,buf,count);
	if (!ret) ret = count;
	return ret;
}

static ssize_t store_val_enum(int id,struct class_device *class_dev,
                              const char *buf,size_t count)
{
	struct pvr2_sysfs *sfp;
	int ret;
	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	ret = store_val_multi(id,sfp,buf,count);
	if (!ret) ret = count;
	return ret;
}

/*
  Mike Isely <isely@pobox.com> 30-April-2005

  This next batch of horrible preprocessor hackery is needed because the
  kernel's class_device_attribute mechanism fails to pass the actual
  attribute through to the show / store functions, which means we have no
  way to package up any attribute-specific parameters, like for example the
  control id.  So we work around this brain-damage by encoding the control
  id into the show / store functions themselves and pick the function based
  on the control id we're setting up.  These macros try to ease the pain.
  Yuck.
*/

#define CREATE_SHOW_INSTANCE(sf_name,ctl_id) \
static ssize_t sf_name##_##ctl_id(struct class_device *class_dev,char *buf) \
{ return sf_name(ctl_id,class_dev,buf); }

#define CREATE_STORE_INSTANCE(sf_name,ctl_id) \
static ssize_t sf_name##_##ctl_id(struct class_device *class_dev,const char *buf,size_t count) \
{ return sf_name(ctl_id,class_dev,buf,count); }

#define CREATE_BATCH(ctl_id) \
CREATE_SHOW_INSTANCE(show_name,ctl_id) \
CREATE_SHOW_INSTANCE(show_min,ctl_id) \
CREATE_SHOW_INSTANCE(show_max,ctl_id) \
CREATE_SHOW_INSTANCE(show_val_int,ctl_id) \
CREATE_SHOW_INSTANCE(show_val_enum,ctl_id) \
CREATE_SHOW_INSTANCE(show_enum,ctl_id) \
CREATE_STORE_INSTANCE(store_val_int,ctl_id) \
CREATE_STORE_INSTANCE(store_val_enum,ctl_id)

CREATE_BATCH(0)
	CREATE_BATCH(1)
	CREATE_BATCH(2)
	CREATE_BATCH(3)
	CREATE_BATCH(4)
	CREATE_BATCH(5)
	CREATE_BATCH(6)
	CREATE_BATCH(7)
	CREATE_BATCH(8)
	CREATE_BATCH(9)
	CREATE_BATCH(10)
	CREATE_BATCH(11)
	CREATE_BATCH(12)
	CREATE_BATCH(13)
	CREATE_BATCH(14)
	CREATE_BATCH(15)
	CREATE_BATCH(16)
	CREATE_BATCH(17)
	CREATE_BATCH(18)
	CREATE_BATCH(19)
	CREATE_BATCH(20)
	CREATE_BATCH(21)
	CREATE_BATCH(22)
	CREATE_BATCH(23)
	CREATE_BATCH(24)
	CREATE_BATCH(25)
	CREATE_BATCH(26)
	CREATE_BATCH(27)
	CREATE_BATCH(28)
	CREATE_BATCH(29)
	CREATE_BATCH(30)
	CREATE_BATCH(31)

	struct pvr2_sysfs_func_set {
		ssize_t (*show_name)(struct class_device *,char *);
		ssize_t (*show_min)(struct class_device *,char *);
		ssize_t (*show_max)(struct class_device *,char *);
		ssize_t (*show_enum)(struct class_device *,char *);
		ssize_t (*show_val_int)(struct class_device *,char *);
		ssize_t (*show_val_enum)(struct class_device *,char *);
		ssize_t (*store_val_int)(struct class_device *,
					 const char *,size_t);
		ssize_t (*store_val_enum)(struct class_device *,
					  const char *,size_t);
	};

#define INIT_BATCH(ctl_id) \
[ctl_id] = { \
    .show_name = show_name_##ctl_id, \
    .show_min = show_min_##ctl_id, \
    .show_max = show_max_##ctl_id, \
    .show_enum = show_enum_##ctl_id, \
    .show_val_int = show_val_int_##ctl_id, \
    .show_val_enum = show_val_enum_##ctl_id, \
    .store_val_int = store_val_int_##ctl_id, \
    .store_val_enum = store_val_enum_##ctl_id, \
} \

static struct pvr2_sysfs_func_set funcs[] = {
	INIT_BATCH(0),
	INIT_BATCH(1),
	INIT_BATCH(2),
	INIT_BATCH(3),
	INIT_BATCH(4),
	INIT_BATCH(5),
	INIT_BATCH(6),
	INIT_BATCH(7),
	INIT_BATCH(8),
	INIT_BATCH(9),
	INIT_BATCH(10),
	INIT_BATCH(11),
	INIT_BATCH(12),
	INIT_BATCH(13),
	INIT_BATCH(14),
	INIT_BATCH(15),
	INIT_BATCH(16),
	INIT_BATCH(17),
	INIT_BATCH(18),
	INIT_BATCH(19),
	INIT_BATCH(20),
	INIT_BATCH(21),
	INIT_BATCH(22),
	INIT_BATCH(23),
	INIT_BATCH(24),
	INIT_BATCH(25),
	INIT_BATCH(26),
	INIT_BATCH(27),
	INIT_BATCH(28),
	INIT_BATCH(29),
	INIT_BATCH(30),
	INIT_BATCH(31),
};


static void pvr2_sysfs_add_control(struct pvr2_sysfs *sfp,int ctl_id)
{
	struct pvr2_sysfs_ctl_item *cip;
	struct pvr2_sysfs_func_set *fp;

	if ((ctl_id < 0) || (ctl_id >= (sizeof(funcs)/sizeof(funcs[0])))) {
		return;
	}

	fp = funcs + ctl_id;

	cip = kmalloc(sizeof(*cip),GFP_KERNEL);
	if (!cip) return;
	memset(cip,0,sizeof(*cip));
	pvr2_sysfs_trace("Creating pvr2_sysfs_ctl_item id=%p",cip);

	cip->attr_id = ctl_id;

	cip->chptr = sfp;
	cip->item_next = 0;
	if (sfp->item_last) {
		sfp->item_last->item_next = cip;
	} else {
		sfp->item_first = cip;
	}
	sfp->item_last = cip;

	cip->attr_name.attr.owner = THIS_MODULE;
	cip->attr_name.attr.name = "name";
	cip->attr_name.attr.mode = S_IRUGO;
	cip->attr_name.show = fp->show_name;

	cip->attr_min.attr.owner = THIS_MODULE;
	cip->attr_min.attr.name = "min_val";
	cip->attr_min.attr.mode = S_IRUGO;
	cip->attr_min.show = fp->show_min;

	cip->attr_max.attr.owner = THIS_MODULE;
	cip->attr_max.attr.name = "max_val";
	cip->attr_max.attr.mode = S_IRUGO;
	cip->attr_max.show = fp->show_max;

	cip->attr_val.attr.owner = THIS_MODULE;
	cip->attr_val.attr.name = "cur_val";
	cip->attr_val.attr.mode = S_IRUGO;

	cip->attr_enum.attr.owner = THIS_MODULE;
	cip->attr_enum.attr.name = "enum_val";
	cip->attr_enum.attr.mode = S_IRUGO;
	cip->attr_enum.show = fp->show_enum;

	if (pvr2_hdw_get_ctl_rw(sfp->channel.hdw,ctl_id)) {
		cip->attr_val.attr.mode |= S_IWUSR|S_IWGRP;
	}

	cip->attr_gen[0] = &cip->attr_name.attr;
	cip->attr_gen[1] = &cip->attr_val.attr;
	if (pvr2_hdw_get_ctl_value_name(
		    sfp->channel.hdw,ctl_id,
		    pvr2_hdw_get_ctl_min_value(sfp->channel.hdw,ctl_id))) {
		// Control is an enumeration
		cip->attr_gen[2] = &cip->attr_enum.attr;
		cip->attr_val.show = fp->show_val_enum;
		cip->attr_val.store = fp->store_val_enum;
	} else {
		// Control is an integer
		cip->attr_val.show = fp->show_val_int;
		cip->attr_val.store = fp->store_val_int;
		cip->attr_gen[2] = &cip->attr_min.attr;
		cip->attr_gen[3] = &cip->attr_max.attr;
	}

	cip->grp.name = item_names[ctl_id];
	cip->grp.attrs = cip->attr_gen;

	sysfs_create_group(&sfp->class_dev->kobj,&cip->grp);
}

static ssize_t debuginfo_show(struct class_device *,char *);
static ssize_t debugcmd_show(struct class_device *,char *);
static ssize_t debugcmd_store(struct class_device *,const char *,size_t count);

static void pvr2_sysfs_add_debugifc(struct pvr2_sysfs *sfp)
{
	struct pvr2_sysfs_debugifc *dip;
	dip = kmalloc(sizeof(*dip),GFP_KERNEL);
	if (!dip) return;
	memset(dip,0,sizeof(*dip));
	dip->attr_debugcmd.attr.owner = THIS_MODULE;
	dip->attr_debugcmd.attr.name = "debugcmd";
	dip->attr_debugcmd.attr.mode = S_IRUGO|S_IWUSR|S_IWGRP;
	dip->attr_debugcmd.show = debugcmd_show;
	dip->attr_debugcmd.store = debugcmd_store;
	dip->attr_debuginfo.attr.owner = THIS_MODULE;
	dip->attr_debuginfo.attr.name = "debuginfo";
	dip->attr_debuginfo.attr.mode = S_IRUGO;
	dip->attr_debuginfo.show = debuginfo_show;
	sfp->debugifc = dip;
	class_device_create_file(sfp->class_dev,&dip->attr_debugcmd);
	class_device_create_file(sfp->class_dev,&dip->attr_debuginfo);
}


static void pvr2_sysfs_tear_down_debugifc(struct pvr2_sysfs *sfp)
{
	if (!sfp->debugifc) return;
	class_device_remove_file(sfp->class_dev,
				 &sfp->debugifc->attr_debuginfo);
	class_device_remove_file(sfp->class_dev,&sfp->debugifc->attr_debugcmd);
	kfree(sfp->debugifc);
	sfp->debugifc = 0;
}


static void pvr2_sysfs_add_controls(struct pvr2_sysfs *sfp)
{
	unsigned int ctl_id;

	for (ctl_id = 0;
	     ctl_id < (sizeof(item_names)/sizeof(item_names[0])); ctl_id++) {
		if (!item_names[ctl_id]) continue;
		pvr2_sysfs_add_control(sfp,ctl_id);
	}
}


static void pvr2_sysfs_tear_down_controls(struct pvr2_sysfs *sfp)
{
	struct pvr2_sysfs_ctl_item *cip1,*cip2;
	for (cip1 = sfp->item_first; cip1; cip1 = cip2) {
		cip2 = cip1->item_next;
		sysfs_remove_group(&sfp->class_dev->kobj,&cip1->grp);
		pvr2_sysfs_trace("Destroying pvr2_sysfs_ctl_item id=%p",cip1);
		kfree(cip1);
	}
}


static void pvr2_sysfs_class_release(struct class *class)
{
	struct pvr2_sysfs_class *clp;
	clp = container_of(class,struct pvr2_sysfs_class,class);
	pvr2_sysfs_trace("Destroying pvr2_sysfs_class id=%p",clp);
	kfree(clp);
}


static void pvr2_sysfs_release(struct class_device *class_dev)
{
	pvr2_sysfs_trace("Releasing class_dev id=%p",class_dev);
	kfree(class_dev);
}


static void class_dev_destroy(struct pvr2_sysfs *sfp)
{
	if (!sfp->class_dev) return;
	pvr2_sysfs_tear_down_debugifc(sfp);
	pvr2_sysfs_tear_down_controls(sfp);
	class_device_remove_file(sfp->class_dev,&sfp->attr_v4l_minor_number);
	class_device_remove_file(sfp->class_dev,&sfp->attr_unit_number);
	pvr2_sysfs_trace("Destroying class_dev id=%p",sfp->class_dev);
	sfp->class_dev->class_data = 0;
	class_device_unregister(sfp->class_dev);
	sfp->class_dev = 0;
}


static ssize_t v4l_minor_number_show(struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	return scnprintf(buf,PAGE_SIZE,"%d\n",
			 pvr2_hdw_v4l_get_minor_number(sfp->channel.hdw));
}


static ssize_t unit_number_show(struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	return scnprintf(buf,PAGE_SIZE,"%d\n",
			 pvr2_hdw_get_unit_number(sfp->channel.hdw));
}


static void class_dev_create(struct pvr2_sysfs *sfp,
                             struct pvr2_sysfs_class *class_ptr)
{
	struct usb_device *usb_dev;
	struct class_device *class_dev;
	usb_dev = pvr2_hdw_get_dev(sfp->channel.hdw);
	if (!usb_dev) return;
	class_dev = kmalloc(sizeof(*class_dev),GFP_KERNEL);
	if (!class_dev) return;
	memset(class_dev,0,sizeof(*class_dev));

	pvr2_sysfs_trace("Creating class_dev id=%p",class_dev);

	class_dev->class = &class_ptr->class;
	if (pvr2_hdw_get_sn(sfp->channel.hdw)) {
		snprintf(class_dev->class_id,BUS_ID_SIZE,"sn-%lu",
			 pvr2_hdw_get_sn(sfp->channel.hdw));
	} else if (pvr2_hdw_get_unit_number(sfp->channel.hdw) >= 0) {
		snprintf(class_dev->class_id,BUS_ID_SIZE,"unit-%c",
			 pvr2_hdw_get_unit_number(sfp->channel.hdw) + 'a');
	} else {
		kfree(class_dev);
		return;
	}

	class_dev->dev = &usb_dev->dev;

	sfp->class_dev = class_dev;
	class_dev->class_data = sfp;
	class_device_register(class_dev);

	sfp->attr_v4l_minor_number.attr.owner = THIS_MODULE;
	sfp->attr_v4l_minor_number.attr.name = "v4l_minor_number";
	sfp->attr_v4l_minor_number.attr.mode = S_IRUGO;
	sfp->attr_v4l_minor_number.show = v4l_minor_number_show;
	sfp->attr_v4l_minor_number.store = 0;
	class_device_create_file(sfp->class_dev,&sfp->attr_v4l_minor_number);
	sfp->attr_unit_number.attr.owner = THIS_MODULE;
	sfp->attr_unit_number.attr.name = "unit_number";
	sfp->attr_unit_number.attr.mode = S_IRUGO;
	sfp->attr_unit_number.show = unit_number_show;
	sfp->attr_unit_number.store = 0;
	class_device_create_file(sfp->class_dev,&sfp->attr_unit_number);

	pvr2_sysfs_add_controls(sfp);
	pvr2_sysfs_add_debugifc(sfp);
}


static void pvr2_sysfs_internal_check(struct pvr2_channel *chp)
{
	struct pvr2_sysfs *sfp;
	sfp = container_of(chp,struct pvr2_sysfs,channel);
	if (!sfp->channel.mc_head->disconnect_flag) return;
	pvr2_trace(PVR2_TRACE_STRUCT,"Destroying pvr2_sysfs id=%p",sfp);
	class_dev_destroy(sfp);
	pvr2_channel_done(&sfp->channel);
	kfree(sfp);
}


struct pvr2_sysfs *pvr2_sysfs_create(struct pvr2_context *mp,
                                     struct pvr2_sysfs_class *class_ptr)
{
	struct pvr2_sysfs *sfp;
	sfp = kmalloc(sizeof(*sfp),GFP_KERNEL);
	if (!sfp) return sfp;
	memset(sfp,0,sizeof(*sfp));
	pvr2_trace(PVR2_TRACE_STRUCT,"Creating pvr2_sysfs id=%p",sfp);
	pvr2_channel_init(&sfp->channel,mp);
	sfp->channel.check_func = pvr2_sysfs_internal_check;

	class_dev_create(sfp,class_ptr);
	return sfp;
}


static int pvr2_sysfs_hotplug(struct class_device *cd,char **envp,
                              int numenvp,char *buf,int size)
{
	/* Even though we don't do anything here, we still need this function
	   because sysfs will still try to call it. */
	return 0;
}

struct pvr2_sysfs_class *pvr2_sysfs_class_create(void)
{
	struct pvr2_sysfs_class *clp;
	clp = kmalloc(sizeof(*clp),GFP_KERNEL);
	if (!clp) return clp;
	memset(clp,0,sizeof(*clp));
	pvr2_sysfs_trace("Creating pvr2_sysfs_class id=%p",clp);
	clp->class.name = "pvrusb2";
	clp->class.class_release = pvr2_sysfs_class_release;
	clp->class.release = pvr2_sysfs_release;
	clp->class.hotplug = pvr2_sysfs_hotplug;
	if (class_register(&clp->class)) {
		pvr2_sysfs_trace(
			"Registration failed for pvr2_sysfs_class id=%p",clp);
		kfree(clp);
		clp = 0;
	}
	return clp;
}


void pvr2_sysfs_class_destroy(struct pvr2_sysfs_class *clp)
{
	class_unregister(&clp->class);
}


static ssize_t debuginfo_show(struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	return pvr2_debugifc_print_info(sfp->channel.hdw,buf,PAGE_SIZE);
}


static ssize_t debugcmd_show(struct class_device *class_dev,char *buf)
{
	struct pvr2_sysfs *sfp;
	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;
	return pvr2_debugifc_print_status(sfp->channel.hdw,buf,PAGE_SIZE);
}


static ssize_t debugcmd_store(struct class_device *class_dev,
                              const char *buf,size_t count)
{
	struct pvr2_sysfs *sfp;
	int ret;

	sfp = (struct pvr2_sysfs *)class_dev->class_data;
	if (!sfp) return -EINVAL;

	ret = pvr2_debugifc_docmd(sfp->channel.hdw,buf,count);
	if (ret < 0) return ret;
	return count;
}


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
