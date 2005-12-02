/*
 *
 *  $Id: pvrusb2-i2c.c,v 1.2 2005/11/27 23:01:16 mcisely Exp $
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

#include "compat.h"
#include "pvrusb2-i2c.h"
#include "pvrusb2-hdw-internal.h"
#include "pvrusb2-tuner.h"
#include "pvrusb2-debug.h"

#define trace_i2c(...) pvr2_trace(PVR2_TRACE_I2C,__VA_ARGS__)

#ifndef I2C_DRIVERID_SAA7115
// Using temporary hack for missing I2C driver-ID for saa7115
#define I2C_DRIVERID_SAA7115 I2C_DRIVERID_EXP1
#endif

#ifndef I2C_DRIVERID_TVEEPROM
// Using temporary hack for missing I2C driver-ID for tveeprom
#define I2C_DRIVERID_TVEEPROM I2C_DRIVERID_EXP2
#endif

/*

  This module attempts to implement a compliant I2C adapter for the pvrusb2
  device.  By doing this we can then make use of existing functionality in
  V4L (e.g. tuner.c) rather than rolling our own.

*/

static unsigned int i2c_scan = 0;
module_param(i2c_scan, int, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(i2c_scan,"scan i2c bus at insmod time");

static int pvr2_i2c_write(struct pvr2_hdw *hdw, /* Context */
                          u8 i2c_addr,      /* I2C address we're talking to */
                          u8 *data,         /* Data to write */
                          u16 length)       /* Size of data to write */
{
	/* Return value - default 0 means success */
	int ret;

#ifdef notdef
	trace_i2c("pvr2_i2c_write");
#endif

	if (!data) length = 0;
	if (length > (sizeof(hdw->cmd_buffer) - 3)) return -ENOTSUPP;

	LOCK_TAKE(hdw->ctl_lock);

	/* Clear the command buffer (likely to be paranoia) */
	memset(hdw->cmd_buffer, 0, sizeof(hdw->cmd_buffer));

	/* Set up command buffer for an I2C write */
	hdw->cmd_buffer[0] = 0x08;      /* write prefix */
	hdw->cmd_buffer[1] = i2c_addr;  /* i2c addr of chip */
	hdw->cmd_buffer[2] = length;    /* length of what follows */
	if (length) memcpy(hdw->cmd_buffer + 3, data, length);

	/* Do the operation */
	ret = pvr2_send_request(hdw,
				hdw->cmd_buffer,
				length + 3,
				hdw->cmd_buffer,
				1);
	if (!ret) {
		if (hdw->cmd_buffer[0] != 8) {
			ret = -EIO;
			if (hdw->cmd_buffer[0] != 7) {
				trace_i2c("unexpected status"
					  " from i2_write[%d]: %d",
					  i2c_addr,hdw->cmd_buffer[0]);
			}
		}
	}
#ifdef notdef
	trace_i2c("i2c_write(%d) len=%d ret=%d stat=%d",i2c_addr,length,ret,
		  hdw->cmd_buffer[0]);
#endif

	LOCK_GIVE(hdw->ctl_lock);

	return ret;
}

static int pvr2_i2c_read(struct pvr2_hdw *hdw, /* Context */
                         u8 i2c_addr,       /* I2C address we're talking to */
                         u8 *data,          /* Data to write */
                         u16 dlen,          /* Size of data to write */
                         u8 *res,           /* Where to put data we read */
                         u16 rlen)          /* Amount of data to read */
{
	/* Return value - default 0 means success */
	int ret;

#ifdef notdef
	trace_i2c("pvr2_i2c_read");
#endif

	if (!data) dlen = 0;
	if (dlen > (sizeof(hdw->cmd_buffer) - 4)) return -ENOTSUPP;
	if (res && (rlen > (sizeof(hdw->cmd_buffer) - 1))) return -ENOTSUPP;

	LOCK_TAKE(hdw->ctl_lock);

	/* Clear the command buffer (likely to be paranoia) */
	memset(hdw->cmd_buffer, 0, sizeof(hdw->cmd_buffer));

	/* Set up command buffer for an I2C write followed by a read */
	hdw->cmd_buffer[0] = 0x09;  /* read prefix */
	hdw->cmd_buffer[1] = dlen;  /* arg length */
	hdw->cmd_buffer[2] = rlen;  /* answer length. Device will send one
				       more byte (status). */
	hdw->cmd_buffer[3] = i2c_addr;  /* i2c addr of chip */
	if (dlen) memcpy(hdw->cmd_buffer + 4, data, dlen);

	/* Do the operation */
	ret = pvr2_send_request(hdw,
				hdw->cmd_buffer,
				4 + dlen,
				hdw->cmd_buffer,
				rlen + 1);
	if (!ret) {
		if (hdw->cmd_buffer[0] != 8) {
			ret = -EIO;
			if (hdw->cmd_buffer[0] != 7) {
				trace_i2c("unexpected status"
					  " from i2_read[%d]: %d",
					  i2c_addr,hdw->cmd_buffer[0]);
			}
		}
	}

#ifdef notdef
	trace_i2c("i2c_read(%d) wlen=%d rlen=%d ret=%d stat=%d",
		  i2c_addr,dlen,rlen,ret,hdw->cmd_buffer[0]);
#endif
	/* Copy back the result */
	if (res && rlen) {
		if (ret) {
			/* Error, just blank out the return buffer */
			memset(res, 0, rlen);
		} else {
			memcpy(res, hdw->cmd_buffer + 1, rlen);
		}
	}

	LOCK_GIVE(hdw->ctl_lock);

	return ret;
}

/* This is a very, very limited I2C adapter implementation.  We can only
   support what we actually know will work on the device... */
static int pvr2_i2c_xfer(struct i2c_adapter *i2c_adap,
                         struct i2c_msg msgs[],
                         int num)
{
	int ret = -ENOTSUPP;
	struct pvr2_hdw *hdw = (struct pvr2_hdw *)(i2c_adap->algo_data);

	if ((msgs[0].flags & I2C_M_NOSTART)) {
		trace_i2c("i2c refusing I2C_M_NOSTART");
		goto done;
	}

	if (num == 1) {
		if (msgs[0].flags & I2C_M_RD) {
			/* Simple read */
			u16 tcnt,bcnt,offs;
			/* If the read is short enough we'll do the whole
			   thing atomically.  Otherwise we have no choice
			   but to break apart the reads. */
			tcnt = msgs[0].len;
			offs = 0;
			while (tcnt) {
				bcnt = tcnt;
				if (bcnt > sizeof(hdw->cmd_buffer)-1) {
					bcnt = sizeof(hdw->cmd_buffer)-1;
				}
				if (pvr2_i2c_read(hdw,msgs[0].addr,
						  0,0,
						  msgs[0].buf+offs,bcnt)) {
					ret = -EIO;
					goto done;
				}
				offs += bcnt;
				tcnt -= bcnt;
			}
			ret = 1;
			goto done;
		} else {
			/* Simple write */
			ret = 1;
			if (pvr2_i2c_write(hdw,msgs[0].addr,
					   msgs[0].buf,msgs[0].len)) {
				ret = -EIO;
			}
			goto done;
		}
	} else if (num == 2) {
		if ((!((msgs[0].flags & I2C_M_RD))) &&
		    (msgs[1].flags & I2C_M_RD)) {
			u16 tcnt,bcnt,wcnt,offs;
			/* Write followed by atomic read.  If the read
			   portion is short enough we'll do the whole thing
			   atomically.  Otherwise we have no choice but to
			   break apart the reads. */
			tcnt = msgs[1].len;
			wcnt = msgs[0].len;
			offs = 0;
			while (tcnt || wcnt) {
				bcnt = tcnt;
				if (bcnt > sizeof(hdw->cmd_buffer)-1) {
					bcnt = sizeof(hdw->cmd_buffer)-1;
				}
				if (pvr2_i2c_read(hdw,msgs[0].addr,
						  msgs[0].buf,wcnt,
						  msgs[1].buf+offs,bcnt)) {
					ret = -EIO;
					goto done;
				}
				offs += bcnt;
				tcnt -= bcnt;
				wcnt = 0;
			}
			ret = 2;
			goto done;
		} else {
			trace_i2c("i2c refusing complex transfer"
				  " read0=%d read1=%d",
				  (msgs[0].flags & I2C_M_RD),
				  (msgs[1].flags & I2C_M_RD));
		}
	} else {
		trace_i2c("i2c refusing %d phase transfer",num);
	}

 done:
	return ret;
}

static int pvr2_i2c_control(struct i2c_adapter *adapter,
                            unsigned int cmd, unsigned long arg)
{
	return 0;
}

static u32 pvr2_i2c_functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_SMBUS_EMUL;
}

static int pvr2_i2c_attach_inform(struct i2c_client *client)
{
	struct pvr2_hdw *hdw = (struct pvr2_hdw *)(client->adapter->algo_data);
	int id;
	id = client->driver->id;
	trace_i2c("i2c_attach [client=%s @ 0x%x id=0x%x]",
		  client->name,
		  client->addr,id);
	if (!(hdw->i2c_audio_client) && (id == I2C_DRIVERID_MSP3400)) {
		trace_i2c("attaching msp3400 I2C client");
		hdw->i2c_audio_client = client;
	}
	if (!(hdw->i2c_tuner_client) && (id == I2C_DRIVERID_TUNER)) {
		trace_i2c("attaching tuner I2C client");
		hdw->i2c_tuner_client = client;
		pvr2_tuner_set_type(hdw);
	}
#ifdef PVR2_ENABLE_SAA7115
	if (!(hdw->i2c_video_client) && (id == I2C_DRIVERID_SAA7115)) {
		trace_i2c("attaching saa7115 I2C client");
		hdw->i2c_video_client = client;
	}
#endif
#ifndef PVR2_SUPPRESS_SAA711X
	if (!(hdw->i2c_video_client) && (id == I2C_DRIVERID_SAA711X)) {
		trace_i2c("attaching saa711x I2C client");
		hdw->i2c_video_client = client;
	}
#endif
	if (!(hdw->i2c_tveeprom_client) && (id == I2C_DRIVERID_TVEEPROM)) {
		trace_i2c("attaching tveeprom I2C client");
		hdw->i2c_tveeprom_client = client;
	}

	return 0;
}

int pvr2_i2c_cmd(struct i2c_client *cp,unsigned int cmd,void *arg)
{
	int stat;
	if (!cp) return -EINVAL;
	if (!(cp->driver)) return -EINVAL;
	if (!(cp->driver->command)) return -EINVAL;
	if (!try_module_get(cp->driver->owner)) return -EAGAIN;
	stat = cp->driver->command(cp,cmd,arg);
	module_put(cp->driver->owner);
	return stat;
}

int pvr2_i2c_tuner_cmd(struct pvr2_hdw *hdw,unsigned int cmd,void *arg)
{
	int stat = pvr2_i2c_cmd(hdw->i2c_tuner_client,cmd,arg);
	if (stat < 0) trace_i2c("pvr2_i2c_tuner_cmd failed with status %d",
				stat);
	return stat;
}

int pvr2_i2c_msp3400_cmd(struct pvr2_hdw *hdw,unsigned int cmd,void *arg)
{
	int stat = pvr2_i2c_cmd(hdw->i2c_audio_client,cmd,arg);
	if (stat < 0) trace_i2c("pvr2_i2c_msp3400_cmd failed with status %d",
				stat);
	return stat;
}

int pvr2_i2c_saa7115_cmd(struct pvr2_hdw *hdw,unsigned int cmd,void *arg)
{
	int stat = pvr2_i2c_cmd(hdw->i2c_video_client,cmd,arg);
	if (stat < 0) trace_i2c("pvr2_i2c_saa7115_cmd failed with status %d",
				stat);
	return stat;
}

int pvr2_i2c_tveeprom_cmd(struct pvr2_hdw *hdw,unsigned int cmd,void *arg)
{
	int stat = pvr2_i2c_cmd(hdw->i2c_tveeprom_client,cmd,arg);
	if (stat < 0) {
		trace_i2c("pvr2_i2c_tveeprom_cmd failed with status %d",stat);
	}
	return stat;
}

static int pvr2_i2c_detach_inform(struct i2c_client *client)
{
	struct pvr2_hdw *hdw = (struct pvr2_hdw *)(client->adapter->algo_data);
	trace_i2c("pvr2_i2c_detach [client=%s @ 0x%x]",
		  client->name,
		  client->addr);
	if (hdw->i2c_audio_client == client) {
		trace_i2c("detaching msp3400 I2C client");
		hdw->i2c_audio_client = 0;
	}
	if (hdw->i2c_tuner_client == client) {
		trace_i2c("detaching tuner I2C client");
		hdw->i2c_tuner_client = 0;
	}
	if (hdw->i2c_video_client == client) {
		trace_i2c("detaching saa7115 / saa711x I2C client");
		hdw->i2c_video_client = 0;
	}
	if (hdw->i2c_tveeprom_client == client) {
		trace_i2c("detaching tveeprom I2C client");
		hdw->i2c_tveeprom_client = 0;
	}

	return 0;
}

static struct i2c_algorithm pvr2_i2c_algo_template = {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
	.id            = I2C_ALGO_BIT | I2C_HW_B_BT848,
#endif
	.master_xfer   = pvr2_i2c_xfer,
	.algo_control  = pvr2_i2c_control,
	.functionality = pvr2_i2c_functionality,
};

static struct i2c_adapter pvr2_i2c_adap_template = {
	.owner         = THIS_MODULE,
	.class	   = I2C_CLASS_TV_ANALOG,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
	.id            = I2C_ALGO_BIT | I2C_HW_B_BT848,
#else
	.id            = I2C_HW_B_BT848,
#endif
	.client_register = pvr2_i2c_attach_inform,
	.client_unregister = pvr2_i2c_detach_inform,
};

static void do_i2c_scan(struct pvr2_hdw *hdw)
{
	struct i2c_msg msg[1];
	int i,rc;
	msg[0].addr = 0;
	msg[0].flags = I2C_M_RD;
	msg[0].len = 0;
	msg[0].buf = 0;
	for (i = 0; i < 128; i++) {
		msg[0].addr = i;
		rc = i2c_transfer(&hdw->i2c_adap,msg,
				  sizeof(msg)/sizeof(msg[0]));
		if (rc < 0) continue;
		printk("%s: i2c scan: found device @ 0x%x\n",hdw->name,i);
	}
}

void pvr2_i2c_init(struct pvr2_hdw *hdw)
{
	memcpy(&hdw->i2c_adap,&pvr2_i2c_adap_template,sizeof(hdw->i2c_adap));
	memcpy(&hdw->i2c_algo,&pvr2_i2c_algo_template,sizeof(hdw->i2c_algo));
	strlcpy(hdw->i2c_adap.name,hdw->name,sizeof(hdw->i2c_adap.name));
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
	strlcpy(hdw->i2c_algo.name,hdw->name,sizeof(hdw->i2c_algo.name));
#endif
	hdw->i2c_adap.algo = &hdw->i2c_algo;
	hdw->i2c_adap.algo_data = hdw;
	i2c_add_adapter(&hdw->i2c_adap);
	hdw->i2c_linked = !0;
	if (i2c_scan) do_i2c_scan(hdw);
}

void pvr2_i2c_done(struct pvr2_hdw *hdw)
{
	if (hdw->i2c_linked) {
		i2c_del_adapter(&hdw->i2c_adap);
		hdw->i2c_linked = 0;
	}
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
