/*
 * 
 * cs53l32a (avc-2010 and avc-2410) i2c driver, well sort of, just initialises the device
 * Martin Vaughan
 *
 * Added code to allow switching of input on Adaptec AVC-2410 by Trev Jackson
 *
 * 
*/


#include <linux/version.h>
#include <linux/module.h>

MODULE_DESCRIPTION("i2c device driver for cs53l32a Audio ADC");
MODULE_AUTHOR("Martin Vaughan");
MODULE_LICENSE("GPL");

#include "compat.h"

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/string.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/videodev.h>
#include <linux/init.h>
#include <linux/smp_lock.h>
#include <asm/pgtable.h>
#include <asm/semaphore.h>
#include "cs53l32a.h"

static int debug = 0;

#ifdef LINUX26
module_param(debug, bool, 0644);
#else
MODULE_PARM(debug, "i");
#endif	/* LINUX26 */

MODULE_PARM_DESC(debug, "Debugging messages\n\t\t\t0=Off (default), 1=On");

#ifndef I2C_DRIVERID_CS53L32A
// Using temporary hack for missing I2C driver-ID for cs53l32a
#define I2C_DRIVERID_EXP4 0xF4
#define I2C_DRIVERID_CS53L32A I2C_DRIVERID_EXP4
#endif

#define CS53L32A_DEBUG(fmt, arg...) \
	do { \
		if (debug) \
	                printk(KERN_INFO "%s debug %d-%04x: " fmt, client->driver->name, \
                               i2c_adapter_id(client->adapter), client->addr , ## arg); \
	} while (0)

#define CS53L32A_INFO(fmt, arg...) do { \
        printk(KERN_INFO "%s %d-%04x: " fmt, client->driver->name, \
               i2c_adapter_id(client->adapter), client->addr , ## arg); } while (0)



static int cs53l32a_attach(struct i2c_adapter *adapter);
static int cs53l32a_detach(struct i2c_client *client);
static int cs53l32a_command(struct i2c_client *client, unsigned int cmd, void *arg);

/* Addresses to scan */
static unsigned short normal_i2c[] = {I2C_CLIENT_END};
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c_range[] = {0x11,0x11,I2C_CLIENT_END};
#endif

I2C_CLIENT_INSMOD;

/* i2c implementation */
#ifndef NEW_I2C
  /* pre i2c-2.8.0 */

static void inc_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif /* MODULE */
}

static void dec_use(struct i2c_client *client)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif /* MODULE */
}

#else
	/* i2c-2.8.0 and later */
#endif /* NEW_I2C */

static struct i2c_driver driver = {
        .name           = "cs53l32a",
        .id             = I2C_DRIVERID_CS53L32A,
        .flags          = I2C_DF_NOTIFY,
        .attach_adapter = cs53l32a_attach,
        .detach_client  = cs53l32a_detach,
        .command        = cs53l32a_command,
#ifndef NEW_I2C
	/* pre i2c-2.8.0 */
	.inc_use = inc_use,
	.dec_use = dec_use,
#else
	/* i2c-2.8.0 and later */
	.owner = THIS_MODULE,
#endif /* NEW_I2C */
};

static inline int cs53l32a_write (struct i2c_client *client, u8 reg, u8 value)
{
	return i2c_smbus_write_byte_data(client, reg, value);
}

static inline int cs53l32a_read (struct i2c_client *client, u8 reg)
{
	return i2c_smbus_read_byte_data(client, reg);
}

static int
cs53l32a_detect_client (struct i2c_adapter *adapter,
                int                 address,
#ifndef LINUX26                       
                unsigned short      flags,
#endif                       
                int                 kind)
{
        struct i2c_client *client;
	int i;

        if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
                return 0;
        }
        client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
        if (client == 0) {
                return -ENOMEM;
        }
        memset(client, 0, sizeof(struct i2c_client));
        client->addr = address;
        client->adapter = adapter;
        client->driver = &driver;
        client->flags = I2C_CLIENT_ALLOW_USE;
	snprintf(client->name, sizeof(client->name) - 1, "cs53l32a");
        
	CS53L32A_INFO("chip found @ 0x%x (%s)\n", address << 1, adapter->name);

	for (i = 1; i <= 7; i++) {
		u8 v = cs53l32a_read(client, i);

        	CS53L32A_DEBUG("Read Reg %d %02x\n", i, v);
	}

        // Set cs53l32a internal register for Adaptec 2010/2410 setup

        cs53l32a_write (client, 0x01, (u8)0x21);
        cs53l32a_write (client, 0x02, (u8)0x29);
        cs53l32a_write (client, 0x03, (u8)0x30);
        cs53l32a_write (client, 0x04, (u8)0x00);
        cs53l32a_write (client, 0x05, (u8)0x00);
        cs53l32a_write (client, 0x06, (u8)0x00);
        cs53l32a_write (client, 0x07, (u8)0x00);

        // Display results, should be 0x21,0x29,0x30,0x00,0x00,0x00,0x00

	for (i = 1; i <= 7; i++) {
		u8 v = cs53l32a_read(client, i);

        	CS53L32A_DEBUG("Read Reg %d %02x\n", i, v);
	}

        i2c_attach_client(client);

        return 0;
}

static int cs53l32a_attach(struct i2c_adapter *adapter)
{
        return i2c_probe(adapter, &addr_data, &cs53l32a_detect_client);	return 0;
}

static int cs53l32a_detach(struct i2c_client *client)
{

        i2c_detach_client(client);

#ifndef LINUX26        
        MOD_DEC_USE_COUNT;
#endif
        return 0;
}

static int cs53l32a_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	int *input = arg;
	
	switch (cmd) {
	case CS53L32A_SET_INPUT:
		if (*input != 0x01 && *input != 0x21) {
			CS53L32A_INFO("Invalid input %d.\n", *input);
			return -EINVAL;
		}
		cs53l32a_write(client, 0x01,(u8)*input);
		break;
	default:
		return -EINVAL;
	}
	return 0;
}



/* ----------------------------------------------------------------------- */

static int cs53l32a_init_module(void)
{
        i2c_add_driver(&driver);
        return 0;
}

static void cs53l32a_cleanup_module(void)
{
        i2c_del_driver(&driver);
}

module_init(cs53l32a_init_module);
module_exit(cs53l32a_cleanup_module);


