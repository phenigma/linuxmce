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
#include "ivtv-dma.h"

/* i2c implementation for iTVC15 chip, ivtv project.
 * Author: Kevin Thayer (nufan_wfk at yahoo.com)
 * License: GPL
 * http://www.sourceforge.net/projects/ivtv/
 */

#ifndef I2C_ADAP_CLASS_TV_ANALOG
  #define I2C_ADAP_CLASS_TV_ANALOG I2C_CLASS_TV_ANALOG
#endif

#define IVTV_SAA7115_I2C_ADDR 		0x21
#define IVTV_SAA7127_I2C_ADDR 		0x44
#define IVTV_TDA9887_I2C_ADDR 		0x43 /* SECAM */
#define IVTV_MSP3400_I2C_ADDR 		0x40
#define IVTV_HAUPPAUGE_I2C_ADDR 	0x50

#ifndef NEW_I2C
/* pre i2c-2.8.0 */
static void ivtv_i2c_inc(struct i2c_adapter *adapter)
{
	IVTV_DEBUG(IVTV_DEBUG_I2C, "i2c increasing usage count\n");
	MOD_INC_USE_COUNT;
}

static void ivtv_i2c_dec(struct i2c_adapter *adapter)
{
	IVTV_DEBUG(IVTV_DEBUG_I2C, "i2c decreasing usage count\n");
	MOD_DEC_USE_COUNT;
}
#endif

static int attach_inform(struct i2c_client *client)
{
        int i; 
	struct ivtv *itv = (struct ivtv*)i2c_get_adapdata(client->adapter);

	IVTV_DEBUG(IVTV_DEBUG_I2C, "i2c client attach\n");
        for (i = 0; i < I2C_CLIENTS_MAX; i++) {
		if (itv->i2c_clients[i] == NULL) {
			itv->i2c_clients[i] = client;
			break;
		}
	}
	IVTV_DEBUG(IVTV_DEBUG_ERR, "i2c attach [client=%s,%s]\n",
		client->name, (i < I2C_CLIENTS_MAX) ?  "ok" : "failed");

	return 0;
}

static int detach_inform(struct i2c_client *client)
{
        int i; 
	struct ivtv *itv = (struct ivtv*)i2c_get_adapdata(client->adapter);

	IVTV_DEBUG(IVTV_DEBUG_I2C, "i2c client detach\n");
        for (i = 0; i < I2C_CLIENTS_MAX; i++) {
		if (itv->i2c_clients[i] == client) {
			itv->i2c_clients[i] = NULL;
			break;
		}
	}
	IVTV_DEBUG(IVTV_DEBUG_I2C, "i2c detach [client=%s,%s]\n",
		client->name, (i < I2C_CLIENTS_MAX) ?  "ok" : "failed");

	return 0;
}

static struct i2c_adapter ivtv_i2c_adapter_template = {
	.name = "ivtv i2c driver",
	.id = I2C_HW_B_BT848,	/* algo-bit is OR'd with this */
	.algo = NULL,		/* set by i2c-algo-bit */
	.algo_data = NULL, 	/* filled from template */
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

static void ivtv_setscl(void *data, int state)
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

static void ivtv_setsda(void *data, int state)
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

static int ivtv_getscl(void *data)
{
	struct ivtv *itv = (struct ivtv *)data;

	return readb(itv->reg_mem + IVTV_REG_I2C_GETSCL_OFFSET);
}

static int ivtv_getsda(void *data)
{
	struct ivtv *itv = (struct ivtv *)data;

	return readb(itv->reg_mem + IVTV_REG_I2C_GETSDA_OFFSET);
}

static struct i2c_algo_bit_data ivtv_i2c_algo_template = {
	NULL,		/* ?? */
	ivtv_setsda,	/* setsda function */
	ivtv_setscl,	/* " */
	ivtv_getsda,	/* " */
	ivtv_getscl,	/* " */
	10,		/* udelay or mdelay */
       	10, 		/* whatever above isn't */
	200		/* timeout */
};

static struct i2c_client ivtv_i2c_client_template = {
	.name = "ivtv internal use only",
	.id   = -1,
};


static int ivtv_call_i2c_client(struct ivtv *itv, int addr, unsigned int cmd, void *arg)
{
	struct i2c_client *client;
        int i;

	down(&itv->i2c_lock);

	IVTV_DEBUG(IVTV_DEBUG_I2C, "call_i2c_client\n");
        for (i = 0; i < I2C_CLIENTS_MAX; i++) {
		client = itv->i2c_clients[i];
                if (client == NULL) {
			IVTV_DEBUG(IVTV_DEBUG_I2C, "no client %d found\n", i);
                        continue;
		}
                if (client->driver->command == NULL) {
                	IVTV_DEBUG(IVTV_DEBUG_I2C,
				   "i2c client with no command interface: "
				   "0x%02x\n", (u8)client->addr);
                        continue;
		}
                if (addr == client->addr) {
			IVTV_DEBUG(IVTV_DEBUG_I2C,
				   "using client %d, addr 0x%02x\n", i, (u8)addr);
                	client->driver->command(client, cmd, arg);
			up(&itv->i2c_lock);
			return 0;
		}
		IVTV_DEBUG(IVTV_DEBUG_I2C,
			   "skipping client %d, addr 0x%02x\n", i, (u8)client->addr);
        }
	IVTV_DEBUG(IVTV_DEBUG_ERR, "i2c client addr: 0x%02x not found!\n", (u8)addr);
	up(&itv->i2c_lock);
	return -ENODEV;
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

int ivtv_tuner(struct ivtv *itv, unsigned int cmd, void *arg)
{
	return ivtv_call_i2c_client(itv, itv->card->i2c_tuner_addr, cmd, arg);
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
	IVTV_DEBUG(IVTV_DEBUG_I2C, "i2c init\n");
	memcpy(&itv->i2c_adap, &ivtv_i2c_adapter_template, sizeof(struct i2c_adapter));
        memcpy(&itv->i2c_algo, &ivtv_i2c_algo_template, sizeof(struct i2c_algo_bit_data));
        memcpy(&itv->i2c_client, &ivtv_i2c_client_template, sizeof(struct i2c_client));

        sprintf(itv->i2c_adap.name + strlen(itv->i2c_adap.name), " #%d", itv->num);
        itv->i2c_algo.data = itv;
        i2c_set_adapdata(&itv->i2c_adap, itv);
        itv->i2c_adap.algo_data = &itv->i2c_algo;
        itv->i2c_client.adapter = &itv->i2c_adap;

	IVTV_DEBUG(IVTV_DEBUG_I2C, "setting scl and sda to 1\n");
        ivtv_setscl(itv, 1);
        ivtv_setsda(itv, 1);

        return i2c_bit_add_bus(&itv->i2c_adap);
}

void __devexit exit_ivtv_i2c(struct ivtv *itv)
{
	IVTV_DEBUG(IVTV_DEBUG_I2C, "i2c exit\n");

	i2c_bit_del_bus(&itv->i2c_adap);
}
