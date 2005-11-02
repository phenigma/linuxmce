/*
    cx25840 driver functions

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

#include <linux/i2c.h>

extern int debug;

void cx25840_write_setting(struct i2c_client *client, u16 setting, u32 value);
u32 cx25840_read_setting(struct i2c_client *client, u16 setting);

int cx25840_write(struct i2c_client *client, u16 addr, u8 value);
int cx25840_write4(struct i2c_client *client, u16 addr, u32 value);
u8 cx25840_read(struct i2c_client *client, u16 addr);
u32 cx25840_read4(struct i2c_client *client, u16 addr);
int cx25840_and_or(struct i2c_client *client, u16 addr, u8 and_mask, u8 or_value);

#define CX25840_DEBUG(fmt,arg...) \
	do { \
		if (debug) \
	                printk(KERN_INFO "%s debug %d-%04x: " fmt, client->driver->name, \
                               i2c_adapter_id(client->adapter), client->addr , ## arg); \
	} while (0)

#define CX25840_ERR(fmt, arg...) do { \
	printk(KERN_ERR "%s %d-%04x: " fmt, client->driver->name, \
               i2c_adapter_id(client->adapter), client->addr , ## arg); } while (0)
#define CX25840_INFO(fmt, arg...) do { \
	printk(KERN_INFO "%s %d-%04x: " fmt, client->driver->name, \
               i2c_adapter_id(client->adapter), client->addr , ## arg); } while (0)

#define VIDEO_IS_PRESENT 1
#define VIDEO_NOT_PRESENT 2

#define CX25840_SVIDEO 0
#define CX25840_COMPOSITE 1
#define CX25840_TUNER 2
#define CX25840_FM 3
#define CX25840_AUDIOIN 4
