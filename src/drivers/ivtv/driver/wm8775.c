/* 
 * wm8775 - driver version 0.0.1
 *
 * Copyright (C) 2004 Ulf Eklund <ivtv at eklund.to>
 *
 * Based on saa7115 driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>
#include <linux/i2c.h>

#include "compat.h"
#include "wm8775.h"

#ifndef LINUX26
#include <linux/wrapper.h>
#endif

#ifndef I2C_DRIVERID_WM8775
// Using temporary hack for missing I2C driver-ID for WM8775
#define I2C_DRIVERID_WM8775 I2C_DRIVERID_EXP1
#endif	/* I2C_DRIVERID_WM8775 */

MODULE_DESCRIPTION("wm8775 driver");
MODULE_AUTHOR("Ulf Eklund");
MODULE_LICENSE("GPL");

static int debug = 0;

#ifdef LINUX26
module_param(debug, bool, 0644);
#else
MODULE_PARM(debug, "i");
#endif	/* LINUX26 */

MODULE_PARM_DESC(debug, "Debugging messages\n\t\t\t0=Off (default), 1=On");

#define WM8775_DEBUG(fmt, arg...) \
	do { \
		if (debug) \
	                printk(KERN_INFO "%s debug %d-%04x: " fmt, client->driver->name, \
                               i2c_adapter_id(client->adapter), client->addr , ## arg); \
	} while (0)

#define WM8775_ERR(fmt, arg...) do { \
	printk(KERN_ERR "%s %d-%04x: " fmt, client->driver->name, \
               i2c_adapter_id(client->adapter), client->addr , ## arg); } while (0)
#define WM8775_INFO(fmt, arg...) do { \
	printk(KERN_INFO "%s %d-%04x: " fmt, client->driver->name, \
               i2c_adapter_id(client->adapter), client->addr , ## arg); } while (0)

/* ----------------------------------------------------------------------- */

enum {
	R7 = 7, R11 = 11,
	R12, R13, R14, R15, R16, R17, R18, R19, R20, R21, R23 = 23,
	TOT_REGS
};

static u16 wm8775_regs_reset[TOT_REGS] = {
	0xffff, 0xffff,		// unused R0, R1
	0xffff, 0xffff,		// unused R2, R3
	0xffff, 0xffff,		// unused R4, R5
	0xffff,			// unused R6
	0x0000,			// R7
	0xffff, 0xffff,		// unused R8, R9
	0xffff,			// unused R10
	0x0022,			// R11
	0x0022,			// R12
	0x0008,			// R13
	0x00CF,			// R14
	0x00CF,			// R15
	0x007B,			// R16
	0x0000,			// R17
	0x0032,			// R18
	0x0000,			// R19
	0x00A6,			// R20
	0x0001,			// R21
	0xffff,			// unused R22
	0x0000			// R23
};

static u16 wm8775_regs[TOT_REGS];

static int wm8775_write(struct i2c_client *client, int reg, u16 val)
{
	int i;

	if (reg < 0 || reg >= TOT_REGS || wm8775_regs[reg] == 0xffff) {
		WM8775_ERR("Invalid register R%d\n", reg);
		return -1;
	}
	// return immediately if the new value equals the old
	if (reg != R23 && val == wm8775_regs[reg])
		return 0;

	for (i = 0; i < 3; i++) {
		if (i2c_smbus_write_byte_data(client, (reg << 1) | (val >> 8),
					      val & 0xff) == 0) {
			if (reg == R23) {
				// Writing R23 resets all registers. So
				// also copy the default values to our
				// copy of the registers.
				memcpy(wm8775_regs, wm8775_regs_reset,
				       sizeof(wm8775_regs));
			} else {
				wm8775_regs[reg] = val;
			}
			return 0;
		}
	}
	WM8775_ERR("I2C Write Problem\n");
	return -1;
}

static int command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	int *input = arg;
        u16 val;

	switch (cmd) {
	case WM8775_SET_INPUT:
		if (*input < 0 || *input > 0xf) {
			WM8775_INFO("Invalid input %d.\n", *input);
			return -EINVAL;
                }
                wm8775_write(client, R21, 0x0c0);
                wm8775_write(client, R14, 0x1d4);
                wm8775_write(client, R15, 0x1d4);
                wm8775_write(client, R21, (*input == 8) ? 0x108 : 0x102);
                break;

	case WM8775_LOG_STATUS:
		WM8775_INFO("Input: %s\n", 
                        wm8775_regs[R21] == 0x108 ? "radio" : "default");
		break;

        case VIDIOCSFREQ:
                val = wm8775_regs[R21];
                wm8775_write(client, R21, 0x0c0);
                wm8775_write(client, R14, 0x1d4);
                wm8775_write(client, R15, 0x1d4);
                wm8775_write(client, R21, val);
                break;

	default:
		return -EINVAL;
	}
	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
#ifndef NEW_I2C
/* pre i2c-2.8.0 */

static void inc_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif	/* MODULE */
}

static void dec_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif	/* MODULE */
}

#else
/* i2c-2.8.0 and later */
#endif	/* NEW_I2C */

#define I2C_WM8775_ADDRESS  0x36

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */
static unsigned short normal_i2c[] =
    { I2C_WM8775_ADDRESS >> 1, I2C_CLIENT_END };
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };
#endif	/* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13) */

I2C_CLIENT_INSMOD;

struct i2c_driver i2c_driver;

static int detect_client(struct i2c_adapter *adapter, int address,
#ifndef LINUX26
			 unsigned short flags,
#endif	/* LINUX26 */
			 int kind)
{
	struct i2c_client *client;

	/* Check if the adapter supports the needed features */
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return 0;

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == 0)
		return -ENOMEM;

	memset(client, 0, sizeof(struct i2c_client));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &i2c_driver;
	client->flags = I2C_CLIENT_ALLOW_USE;
	snprintf(client->name, sizeof(client->name) - 1, "wm8775");
	
	WM8775_INFO("chip found @ 0x%x (%s)\n", address << 1, adapter->name);

        // initialize wm8775
        wm8775_write(client, R23, 0x000); // RESET
        wm8775_write(client, R7,  0x000); // Disable zero cross detect timeout
        wm8775_write(client, R11, 0x021); // Left justified, 24-bit mode
        wm8775_write(client, R12, 0x102); // Master mode, clock ratio 256fs
        wm8775_write(client, R13, 0x000); // Powered up
        wm8775_write(client, R14, 0x1d4); // ADC gain +2.5dB, enable zero cross
        wm8775_write(client, R15, 0x1d4); // ADC gain +2.5dB, enable zero cross
        wm8775_write(client, R16, 0x1bf); // ALC Stereo, ALC target level -1dB FS
                                          // max gain +8dB
        wm8775_write(client, R17, 0x185); // Enable gain control, use zero cross
                                          // detection, ALC hold time 42.6 ms
        wm8775_write(client, R18, 0x0a2); // ALC gain ramp up delay 34 s,
                                          // ALC gain ramp down delay 33 ms
        wm8775_write(client, R19, 0x005); // Enable noise gate, threshold -72dBfs
        wm8775_write(client, R20, 0x07a); // Transient window 4ms, lower PGA gain
                                          // limit -1dB
        wm8775_write(client, R21, 0x102); // LRBOTH = 1, use input 2.
	i2c_attach_client(client);

	return 0;
}

static int attach_adapter(struct i2c_adapter *adapter)
{
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13)
	if (adapter->id == I2C_HW_B_BT848) {
#else
	if (adapter->id == (I2C_ALGO_BIT | I2C_HW_B_BT848)) {
#endif
		return i2c_probe(adapter, &addr_data, &detect_client);
	}

	return 0;
}

static int detach_client(struct i2c_client *client)
{
	int err;

	err = i2c_detach_client(client);
	if (err) {
		return err;
	}

	kfree(client);
	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
struct i2c_driver i2c_driver = {
	.name = "wm8775",

	.id = I2C_DRIVERID_WM8775,
	.flags = I2C_DF_NOTIFY,

	.attach_adapter = attach_adapter,
	.detach_client = detach_client,
	.command = command,
#ifndef NEW_I2C
	/* pre i2c-2.8.0 */
	.inc_use = inc_use,
	.dec_use = dec_use,
#else
	/* i2c-2.8.0 and later */
	.owner = THIS_MODULE,
#endif	/* NEW_I2C */
};

#ifndef LINUX26
EXPORT_NO_SYMBOLS;
#endif	/* LINUX26 */

static int __init minit(void)
{
	return i2c_add_driver(&i2c_driver);
}

static void __exit mexit(void)
{
	i2c_del_driver(&i2c_driver);
}

module_init(minit);
module_exit(mexit);
