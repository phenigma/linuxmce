/*
    I2C functions
    Copyright (C) 2003-2004  Kevin Thayer <nufan_wfk at yahoo.com>

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
#include "ivtv-cards.h"

/* i2c implementation for cx23415/6 chip, ivtv project.
 * Author: Kevin Thayer (nufan_wfk at yahoo.com)
 */

#ifndef I2C_ADAP_CLASS_TV_ANALOG
#define I2C_ADAP_CLASS_TV_ANALOG I2C_CLASS_TV_ANALOG
#endif /* I2C_ADAP_CLASS_TV_ANALOG */

#define IVTV_CS53L32A_I2C_ADDR		0x11
#define IVTV_CX25840_I2C_ADDR 		0x44
#define IVTV_SAA7115_I2C_ADDR 		0x21
#define IVTV_SAA7127_I2C_ADDR 		0x44
#define IVTV_TDA9887_I2C_ADDR 		0x43	/* SECAM */
#define IVTV_MSP3400_I2C_ADDR 		0x40
#define IVTV_HAUPPAUGE_I2C_ADDR 	0x50
#define IVTV_WM8775_I2C_ADDR		0x1B
#define IVTV_TEA5767_I2C_ADDR		0x60

#ifndef NEW_I2C
/* pre i2c-2.8.0 */
static void ivtv_i2c_inc(struct i2c_adapter *adapter)
{
	MOD_INC_USE_COUNT;
}

static void ivtv_i2c_dec(struct i2c_adapter *adapter)
{
	MOD_DEC_USE_COUNT;
}
#endif /* NEW_I2C */

static int attach_inform(struct i2c_client *client)
{
	struct ivtv *itv = (struct ivtv *)i2c_get_adapdata(client->adapter);
        u32 addr = client->addr;
	int i;

	IVTV_DEBUG_I2C("i2c client attach\n");
	for (i = 0; i < I2C_CLIENTS_MAX; i++) {
		if (itv->i2c_clients[i] == NULL) {
			itv->i2c_clients[i] = client;
			break;
		}
	}
        if (addr == IVTV_TDA9887_I2C_ADDR)
                itv->has_tda9887 = 1;
        if (client->driver && client->driver->id == I2C_DRIVERID_TUNER ) {
                if (itv->i2c_tv_tuner_addr) {
                        // We found a second tuner. One of them is the
                        // radio tuner TEA5767, the other is the TV tuner.
                        if (addr == IVTV_TEA5767_I2C_ADDR) {
                                itv->i2c_radio_tuner_addr = addr;
                        }
                        else {
                                itv->i2c_radio_tuner_addr = itv->i2c_tv_tuner_addr;
                                itv->i2c_tv_tuner_addr = addr;
                        }
                }
                else {
                        itv->i2c_tv_tuner_addr = addr;
                }
        }
	IVTV_INFO("i2c attach to card #%d %s [client=%s, addr=%02x]\n",
		   itv->num, (i < I2C_CLIENTS_MAX) ? "ok" : "failed", client->name, addr);

	return 0;
}

static int detach_inform(struct i2c_client *client)
{
	int i;
	struct ivtv *itv = (struct ivtv *)i2c_get_adapdata(client->adapter);

	IVTV_DEBUG_I2C("i2c client detach\n");
	for (i = 0; i < I2C_CLIENTS_MAX; i++) {
		if (itv->i2c_clients[i] == client) {
			itv->i2c_clients[i] = NULL;
			break;
		}
	}
	IVTV_DEBUG_I2C("i2c detach [client=%s,%s]\n",
		   client->name, (i < I2C_CLIENTS_MAX) ? "ok" : "failed");

	return 0;
}

static void ivtv_setscl(struct ivtv *itv, int state)
{
	/* write them out */
	/* write bits are inverted */
	writel(~state, itv->reg_mem + IVTV_REG_I2C_SETSCL_OFFSET);
}

static void ivtv_setsda(struct ivtv *itv, int state)
{
	/* write them out */
	/* write bits are inverted */
	writel(~state & 1, itv->reg_mem + IVTV_REG_I2C_SETSDA_OFFSET);
}

static int ivtv_getscl(struct ivtv *itv)
{
	return readl(itv->reg_mem + IVTV_REG_I2C_GETSCL_OFFSET) & 1;
}

static int ivtv_getsda(struct ivtv *itv)
{
	return readl(itv->reg_mem + IVTV_REG_I2C_GETSDA_OFFSET) & 1;
}

static void ivtv_scldelay(struct ivtv *itv)
{
	int i;
	for (i = 0; i < 5; ++i)
		(void) ivtv_getscl(itv);
}

static int ivtv_waitscl(struct ivtv *itv, int val)
{
	int i;
	
	ivtv_scldelay(itv);
	for (i = 0; i < 1000; ++i) {
		if (ivtv_getscl(itv) == val)
			return 1;
	}
	return 0;
}

static int ivtv_waitsda(struct ivtv *itv, int val)
{
	int i;
	
	ivtv_scldelay(itv);
	for (i = 0; i < 1000; ++i) {
		if (ivtv_getsda(itv) == val)
			return 1;
	}
	return 0;
}

static int ivtv_ack(struct ivtv *itv)
{
	int ret = 0;
	
	if (ivtv_getscl(itv) == 1) {
		IVTV_DEBUG_I2C("SCL was high starting an ack\n");
		ivtv_setscl(itv, 0);
		if (!ivtv_waitscl(itv, 0)) {
			IVTV_DEBUG_I2C("Could not set SCL low starting an ack\n");
			return -EREMOTEIO;
		}
	}
	ivtv_setsda(itv, 1);
	ivtv_scldelay(itv);
	ivtv_setscl(itv, 1);
	if (!ivtv_waitsda(itv, 0)) {
		IVTV_DEBUG_I2C("Slave did not ack\n");
		ret = -EREMOTEIO;
	}
	ivtv_setscl(itv, 0);
	if (!ivtv_waitscl(itv, 0)) {
		IVTV_DEBUG_I2C("Failed to set SCL low after ACK\n");
		ret = -EREMOTEIO;
	}
	return ret;
}

static int ivtv_sendbyte(struct ivtv *itv, unsigned char byte)
{
	int i, bit;
	
	IVTV_DEBUG_I2C("write %x\n",byte);
	for (i = 0; i < 8; ++i, byte<<=1) {
		ivtv_setscl(itv, 0);
		if (!ivtv_waitscl(itv, 0)) {
			IVTV_DEBUG_I2C("Error setting SCL low\n");
			return -EREMOTEIO;
		}
		bit = (byte>>7)&1;
		ivtv_setsda(itv, bit);
		if (!ivtv_waitsda(itv, bit)) {
			IVTV_DEBUG_I2C("Error setting SDA\n");
			return -EREMOTEIO;
		}
		ivtv_setscl(itv, 1);
		if (!ivtv_waitscl(itv, 1)) {
			IVTV_DEBUG_I2C("Slave not ready for bit\n");
			return -EREMOTEIO;
		}
	}
	ivtv_setscl(itv, 0);
	if (!ivtv_waitscl(itv, 0)) {
		IVTV_DEBUG_I2C("Error setting SCL low\n");
		return -EREMOTEIO;
	}
	return ivtv_ack(itv);
}

static int ivtv_readbyte(struct ivtv *itv, unsigned char *byte, int nack)
{
	int i;
	
	*byte = 0;
	
	ivtv_setsda(itv, 1);
	ivtv_scldelay(itv);
	for (i = 0; i < 8; ++i) {
		ivtv_setscl(itv, 0);
		ivtv_scldelay(itv);
		ivtv_setscl(itv, 1);
		if (!ivtv_waitscl(itv, 1)) {
			IVTV_DEBUG_I2C("Error setting SCL high\n");
			return -EREMOTEIO;
		}
		*byte = ((*byte)<<1)|ivtv_getsda(itv);
	}
	ivtv_setscl(itv, 0);
	ivtv_scldelay(itv);
	ivtv_setsda(itv, nack);
	ivtv_scldelay(itv);
	ivtv_setscl(itv, 1);
	ivtv_scldelay(itv);
	ivtv_setscl(itv, 0);
	ivtv_scldelay(itv);
	IVTV_DEBUG_I2C("read %x\n",*byte);
	return 0;
}

static int ivtv_start(struct ivtv *itv)
{
	int sda; 
	
	sda = ivtv_getsda(itv);
	if (sda != 1) {
		IVTV_DEBUG_I2C("SDA was low at start\n");
		ivtv_setsda(itv, 1);
		if (!ivtv_waitsda(itv, 1)) {
			IVTV_DEBUG_I2C("SDA stuck low\n");
			return -EREMOTEIO;
		}
	}
	if (ivtv_getscl(itv) != 1) {
		ivtv_setscl(itv, 1);
		if (!ivtv_waitscl(itv, 1)) {
			IVTV_DEBUG_I2C("SCL stuck low at start\n");
			return -EREMOTEIO;
		}
	}
	ivtv_setsda(itv, 0);
	ivtv_scldelay(itv);
	return 0;
}


static int ivtv_stop(struct ivtv *itv)
{
	int i;
	
	if (ivtv_getscl(itv) != 0) {
		IVTV_DEBUG_I2C("SCL not low when stopping\n");
		ivtv_setscl(itv, 0);
		if (!ivtv_waitscl(itv, 0)) {
			IVTV_DEBUG_I2C("SCL could not be set low\n");
		}
	}
	ivtv_setsda(itv, 0);
	ivtv_scldelay(itv);
	ivtv_setscl(itv, 1);
	if (!ivtv_waitscl(itv, 1)) {
		IVTV_DEBUG_I2C("SCL could not be set high\n");
		return -EREMOTEIO;
	}
	ivtv_scldelay(itv);
	ivtv_setsda(itv, 1);
	if (!ivtv_waitsda(itv, 1)) {
		IVTV_DEBUG_I2C("resetting I2C\n");
		for (i = 0; i < 16; ++i) {
			ivtv_setscl(itv, 0);
			ivtv_scldelay(itv);
			ivtv_setscl(itv, 1);
			ivtv_scldelay(itv);
			ivtv_setsda(itv, 1);
		}
		ivtv_waitsda(itv, 1);
		return -EREMOTEIO;
	}
	return 0;
}

static int ivtv_write(struct ivtv *itv, unsigned char addr, unsigned char *data, u32 len, int do_stop)
{
	int retry, ret = -EREMOTEIO;
	u32 i;
	
	for (retry = 0; ret != 0 && retry < 8; ++retry) {
		ret = ivtv_start(itv);
		
		if (ret == 0) {
			ret = ivtv_sendbyte(itv, addr<<1);
			for (i = 0; ret == 0 && i < len; ++i)
				ret = ivtv_sendbyte(itv, data[i]);
		}
		if (ret != 0 || do_stop) {
			(void) ivtv_stop(itv);
		}
	}
	if (ret == 0) {
		IVTV_DEBUG_I2C("i2c write to %x succeeded\n", addr);
	} else {
		IVTV_DEBUG_I2C("i2c write to %x failed\n", addr);
	}
	return ret;
}

static int ivtv_read(struct ivtv *itv, unsigned char addr, unsigned char *data, u32 len)
{
	int retry, ret = -EREMOTEIO;
	u32 i;
	
	for (retry = 0; ret != 0 && retry < 8; ++retry) {
		ret = ivtv_start(itv);
		if (ret == 0)
			ret = ivtv_sendbyte(itv, (addr << 1) | 1);
		for (i = 0; ret == 0 && i < len; ++i) {
			ret = ivtv_readbyte(itv, &data[i], i == len - 1);
		}
		(void) ivtv_stop(itv);
	}
	if (ret == 0) {
		IVTV_DEBUG_I2C("i2c read from %x succeeded\n", addr);
	} else {
		IVTV_DEBUG_I2C("i2c read from %x failed\n", addr);
	}
	return ret;
}

static int ivtv_xfer(struct i2c_adapter *i2c_adap, struct i2c_msg *msgs, int num)
{
	struct ivtv *itv = i2c_get_adapdata(i2c_adap);
	int retval = 0;
	int i;

	down(&itv->i2c_bus_lock);
	for (i = 0 ; i < num; i++) {
		if (msgs[i].flags & I2C_M_RD) {
			/* read */
			IVTV_DEBUG_I2C("xfer: read %d/%d\n", i, num);
			retval = ivtv_read(itv, msgs[i].addr, msgs[i].buf, msgs[i].len);
			if (retval < 0)
				goto err;
		} else {
			/* if followed by a read, don't stop */
			int stop;
			stop = !(i + 1 < num && msgs[i + 1].flags == I2C_M_RD);

			/* write */
			IVTV_DEBUG_I2C("xfer: write %d/%d, stop %d\n", i, num, stop);
			retval = ivtv_write(itv, msgs[i].addr, msgs[i].buf, msgs[i].len, stop);
			if (retval < 0)
				goto err;
		}
	}
	up(&itv->i2c_bus_lock);
	return num;

err:
 	up(&itv->i2c_bus_lock);
	return retval;
}

static u32 functionality(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static struct i2c_algorithm ivtv_algo = {
	.master_xfer   = ivtv_xfer,
	.functionality = functionality,
};

/* template for our-bit banger */
static struct i2c_adapter ivtv_i2c_adap_hw_template = {
	.name = "ivtv i2c driver",
	.id = I2C_ALGO_BIT|I2C_HW_B_BT848,	/* algo-bit is OR'd with this */
	.algo = &ivtv_algo,
	.algo_data = NULL,			/* filled from template */
	.client_register = attach_inform,
	.client_unregister = detach_inform,
#ifndef NEW_I2C
/* pre i2c-2.8.0 */
	.inc_use = ivtv_i2c_inc,	/* inc usage */
	.dec_use = ivtv_i2c_dec,	/* dec usage */
#else
/* i2c-2.8.0 and later */
	.owner = THIS_MODULE,
#endif /* NEW_I2C */
#ifdef LINUX26
	.class = I2C_ADAP_CLASS_TV_ANALOG,
#endif /* LINUX26 */
};


static void ivtv_setscl_old(void *data, int state)
{
	struct ivtv *itv = (struct ivtv *)data;

	if (state)
		itv->i2c_state |= 0x01;
	else
		itv->i2c_state &= ~0x01;

	/* write them out */
	/* write bits are inverted */
	writel(~itv->i2c_state, itv->reg_mem + IVTV_REG_I2C_SETSCL_OFFSET);
}

static void ivtv_setsda_old(void *data, int state)
{
	struct ivtv *itv = (struct ivtv *)data;

	if (state)
		itv->i2c_state |= 0x01;
	else
		itv->i2c_state &= ~0x01;

	/* write them out */
	/* write bits are inverted */
	writel(~itv->i2c_state, itv->reg_mem + IVTV_REG_I2C_SETSDA_OFFSET);
}

static int ivtv_getscl_old(void *data)
{
	struct ivtv *itv = (struct ivtv *)data;

	return readb(itv->reg_mem + IVTV_REG_I2C_GETSCL_OFFSET);
}

static int ivtv_getsda_old(void *data)
{
	struct ivtv *itv = (struct ivtv *)data;

	return readb(itv->reg_mem + IVTV_REG_I2C_GETSDA_OFFSET);
}

/* template for i2c-bit-algo */
static struct i2c_adapter ivtv_i2c_adap_template = {
	.name = "ivtv i2c driver",
	.id = I2C_ALGO_BIT|I2C_HW_B_BT848,	/* algo-bit is OR'd with this */
	.algo = NULL,				/* set by i2c-algo-bit */
	.algo_data = NULL,			/* filled from template */
	.client_register = attach_inform,
	.client_unregister = detach_inform,
#ifndef NEW_I2C
/* pre i2c-2.8.0 */
	.inc_use = ivtv_i2c_inc,	/* inc usage */
	.dec_use = ivtv_i2c_dec,	/* dec usage */
#else
/* i2c-2.8.0 and later */
	.owner = THIS_MODULE,
#endif
#ifdef LINUX26
	.class = I2C_ADAP_CLASS_TV_ANALOG,
#endif
};

static struct i2c_algo_bit_data ivtv_i2c_algo_template = {
	NULL,			/* ?? */
	ivtv_setsda_old,	/* setsda function */
	ivtv_setscl_old,	/* " */
	ivtv_getsda_old,	/* " */
	ivtv_getscl_old,	/* " */
	10,			/* udelay or mdelay */
	10,			/* whatever above isn't */
	200			/* timeout */
};

static struct i2c_client ivtv_i2c_client_template = {
	.name = "ivtv internal use only",
};



static int ivtv_call_i2c_client(struct ivtv *itv, int addr, unsigned int cmd,
				void *arg)
{
	struct i2c_client *client;
	int i;
	int retval;

	down(&itv->i2c_lock);

	IVTV_DEBUG_I2C("call_i2c_client\n");
	for (i = 0; i < I2C_CLIENTS_MAX; i++) {
		client = itv->i2c_clients[i];
		if (client == NULL) {
			IVTV_DEBUG_I2C("no client %d found\n", i);
			continue;
		}
		if (client->driver->command == NULL) {
			IVTV_DEBUG_I2C(
				   "i2c client with no command interface: "
				   "0x%02x\n", (u8) client->addr);
			continue;
		}
		if (addr == client->addr) {
			IVTV_DEBUG_I2C(
				   "using client %d, addr 0x%02x\n", i,
				   (u8) addr);
			retval = client->driver->command(client, cmd, arg);
			up(&itv->i2c_lock);
			return retval;
		}
		IVTV_DEBUG_I2C(
			   "skipping client %d, addr 0x%02x\n", i,
			   (u8) client->addr);
	}
	IVTV_DEBUG_WARN("i2c client addr: 0x%02x not found for command 0x%x!\n",
		   (u8) addr, cmd);
	up(&itv->i2c_lock);
	return -ENODEV;
}

int ivtv_wm8775(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, IVTV_WM8775_I2C_ADDR, cmd, arg);
}

int ivtv_cs53l32a(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, IVTV_CS53L32A_I2C_ADDR, cmd, arg);
}

int ivtv_cx25840(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, IVTV_CX25840_I2C_ADDR, cmd, arg);
}

int ivtv_saa7115(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, IVTV_SAA7115_I2C_ADDR, cmd, arg);
}

int ivtv_saa7127(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, IVTV_SAA7127_I2C_ADDR, cmd, arg);
}

int ivtv_msp34xx(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, IVTV_MSP3400_I2C_ADDR, cmd, arg);
}

int ivtv_radio_tuner(struct ivtv *itv, unsigned int cmd, void *arg)
{
        u32 addr = itv->i2c_radio_tuner_addr ? itv->i2c_radio_tuner_addr :
                                               itv->i2c_tv_tuner_addr;

	return ivtv_call_i2c_client(itv, addr, cmd, arg);
}

int ivtv_tv_tuner(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, itv->i2c_tv_tuner_addr, cmd, arg);
}

int ivtv_tda9887(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, IVTV_TDA9887_I2C_ADDR, cmd, arg);
}

int ivtv_hauppauge(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, IVTV_HAUPPAUGE_I2C_ADDR, cmd, arg);
}

/* init + register i2c algo-bit adapter */
int __devinit init_ivtv_i2c(struct ivtv *itv)
{
	IVTV_DEBUG_I2C("i2c init\n");
 
 	if (itv->options.newi2c) {
 		memcpy(&itv->i2c_adap, &ivtv_i2c_adap_hw_template,
 		       sizeof(struct i2c_adapter));
 	} else {
 		memcpy(&itv->i2c_adap, &ivtv_i2c_adap_template,
 		       sizeof(struct i2c_adapter));
 		memcpy(&itv->i2c_algo, &ivtv_i2c_algo_template,
 		       sizeof(struct i2c_algo_bit_data));
 		itv->i2c_algo.data = itv;
		itv->i2c_adap.algo_data = &itv->i2c_algo;
	}

	sprintf(itv->i2c_adap.name + strlen(itv->i2c_adap.name), " #%d",
		itv->num);
	i2c_set_adapdata(&itv->i2c_adap, itv);

	memcpy(&itv->i2c_client, &ivtv_i2c_client_template,
	       sizeof(struct i2c_client));
	itv->i2c_client.adapter = &itv->i2c_adap;
  
	IVTV_DEBUG_I2C("setting scl and sda to 1\n");
	ivtv_setscl_old(itv, 1);
	ivtv_setsda_old(itv, 1);

	if (itv->options.newi2c)
		return i2c_add_adapter(&itv->i2c_adap);
	else
		return i2c_bit_add_bus(&itv->i2c_adap);
}

void __devexit exit_ivtv_i2c(struct ivtv *itv)
{
	IVTV_DEBUG_I2C("i2c exit\n");

	if (itv->options.newi2c) {
		i2c_del_adapter(&itv->i2c_adap);
	} else {
		i2c_bit_del_bus(&itv->i2c_adap);
	}
}
