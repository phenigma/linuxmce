/* 
 * upd6408x - NEC Electronics 3-Dimensional Y/C separation LSI driver version 0.0.1
 *
 * 2003 by T.Adachi (tadachi@tadachi-net.com)
 */

#include "ivtv-compat.h"
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include <linux/i2c.h>
#include <linux/videodev2.h>
#include <media/v4l2-common.h>

#include "upd640xx.h"

MODULE_DESCRIPTION("uPD64083 Driver");
MODULE_AUTHOR("T. Adachi <tadachi@tadachi-net.com>, Takeru KOMORIYA <komoriya@paken.org>");
MODULE_LICENSE("GPL");

static int debug = 0;
static int card = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
module_param(debug, bool, 0644);
module_param(card, int, 0644);
#else
MODULE_PARM(debug, "i");
MODULE_PARM(card, "i");
#endif

MODULE_PARM_DESC(debug, "Debug level (0-1)");
MODULE_PARM_DESC(card, "Card Type (integer)");

static unsigned short normal_i2c[] = { 0xb8 >> 1, 0xba >> 1, I2C_CLIENT_END };

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };
#endif

I2C_CLIENT_INSMOD;

// ---------------------------------
#define UPD64032_INPUT_TYPE_MAX    2
// - bit mask ----------------------
#define YCS_MODE_MASK              0xc0 // SA00 bit 7-6
#define USE_EXTERNAL_Y_ADC_MASK    0x20 // SA02 bit 5

#define TUN_YCS_MODE_MASK           0x03
#define COMP_YCS_MODE_MASK          0x0c
#define SVID_YCS_MODE_MASK          0x30

// ---------------------------------

enum {
        R00 = 0, R01, R02, R03, R04,
	R05, R06, R07, R08, R09,
	R0A, R0B, R0C, R0D, R0E, R0F,
	R10, R11, R12, R13, R14,
	R15, R16, 
        TOT_REGS
};

struct upd64083_regdata_st {
	char  *name;
	u8    regs[TOT_REGS];
};

struct upd64083_config_st {
	unsigned int def_set_num;
	u8 current_input_type;
	struct upd64083_regdata_st *data;

	u8 linein_ycs_mode[UPD64032_INPUT_TYPE_MAX];
	u8 linein_y_adc[UPD64032_INPUT_TYPE_MAX];
};

static struct upd64083_regdata_st upd64083_norms[] = {
  {
    .name = "GR_DIG_CONNECT_SETTING",
    .regs = {0x1f, 0x01, 0xa0, 0x2d, 0x29,  // tadachi use EXCSS : 0x00
	     0x36, 0xdd, 0x05, 0x56, 0x48,
	     0x00, 0x3a, 0xa0, 0x05, 0x08,
	     0x44, 0x60, 0x08, 0x52, 0xf8,
	     0x53, 0x60, 0x10
    }
  },
  {
    .name = "NO_GR_DIG_CONNECT_SETTING",
    .regs = {0x5f, 0x01, 0x80, 0x2d, 0x29,
	     0x36, 0xdd, 0x05, 0x56, 0x48,
	     0x00, 0x3a, 0xa0, 0x05, 0x08,
	     0x44, 0x60, 0x08, 0x52, 0xf8,
	     0x53, 0x60, 0x10
    }
  }
};

static int def_set_max = ARRAY_SIZE(upd64083_norms);

#ifndef I2C_DRIVERID_UPD64083
#define I2C_DRIVERID_UPD64083 I2C_DRIVERID_EXP1
#endif

/* ------------------------------------------------------------------------ */

static void upd64083_log_status(struct i2c_client *client)
{
	u8 buf[7];

	i2c_master_recv(client, buf, 7);
	v4l_info(client, "Status: SA00=%02x SA01=%02x SA02=%02x SA03=%02x "
		      "SA04=%02x SA05=%02x SA06=%02x\n",
		buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
}

/* ------------------------------------------------------------------------ */

static int upd64083_write(struct i2c_client *client, u8 reg, u8 val)
{
	u8 buf[2];

	buf[0] = reg;
	buf[1] = val;

        v4l_dbg(1, debug, client, "writing reg addr: %02X val: %02X\n",
                reg, val);
	if (i2c_master_send(client, buf, 2) == 2)
		return 0;
	v4l_err(client, "I/O error write 0x%02x/0x%02x\n", reg, val);
	return -1;
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static u8 upd64083_read(struct i2c_client *client, u8 reg)
{
	u8 buf[7];

	if (reg >= sizeof(buf))
	       	return 0xff;
	i2c_master_recv(client, buf, sizeof(buf));
	return buf[reg];
}
#endif

static int write_all_regs(struct i2c_client *client)
{
        struct upd64083_config_st *config = i2c_get_clientdata(client);
	int i;

	v4l_dbg(1, debug, client, "writing all registers\n");
	for (i = 0; i < TOT_REGS; i++) {
		if (upd64083_write(client, i, config->data->regs[i]) != 0)
		       	return -1;
	}

	return 0;
}

/* ------------------------------------------------------------------------ */

// change input (type)
//   0:tuner 1:composite 2:s-video)
static int chg_input(struct i2c_client *client)
{
	struct upd64083_config_st *config = i2c_get_clientdata(client);
	static const char *input_name[] = {"Tuner","Composite","S-Video"};
	u8 r00,r02;
	int rc;

	switch (config->current_input_type)  {
	case 0: // tuner
		r00 = config->data->regs[R00];
		r02 = config->data->regs[R02];
		break;
	case 1: // composite
	case 2: // S-video
		r00 = (config->data->regs[R00] & ~YCS_MODE_MASK) |
			config->linein_ycs_mode[config->current_input_type - 1];
		r02 =  (config->data->regs[R02] & ~USE_EXTERNAL_Y_ADC_MASK) |
			config->linein_y_adc[config->current_input_type - 1];
		break;
	default:
		return -ERANGE;
	}

	v4l_dbg(1, debug, client, "change input to %s\n",
			input_name[config->current_input_type]);

	rc = upd64083_write(client,R00,r00);
	if (rc != 0)
		return rc;
	return upd64083_write(client,R02,r02);
}

/* ------------------------------------------------------------------------ */

static int upd64083_set_norm(struct i2c_client *client)
{
	struct upd64083_config_st *config = i2c_get_clientdata(client);
	int i;

	if (config->def_set_num > def_set_max)
		return -ERANGE;

	for (i = 0; i < TOT_REGS; i++) {
		config->data->regs[i] = upd64083_norms[config->def_set_num].regs[i];
	}

	config->current_input_type = 0;

	// composite
	config->linein_ycs_mode[0] = 
		(config->data->regs[R00] & YCS_MODE_MASK);
	config->linein_y_adc[0] = 0x80;

	// S-Video
	config->linein_ycs_mode[1] = 0xc0;
	config->linein_y_adc[1] = 0x80;

	v4l_dbg(1, debug, client, "set norm to [%d]\n",
			config->def_set_num);

	return 0;
}

/* ------------------------------------------------------------------------ */

static void upd64083_set_param1_configure(struct i2c_client *client, u32 value)
{
	struct upd64083_config_st *config = i2c_get_clientdata(client);
	u8  *tun_reg, *comp_reg, *svid_reg;

	// YCS MODE -----
	tun_reg = &config->data->regs[R00];
	comp_reg = &config->linein_ycs_mode[0];
	svid_reg = &config->linein_ycs_mode[1];

	if (value & UPD64083_SET_TUN_YCSMODE) 
		*tun_reg = (*tun_reg & ~YCS_MODE_MASK) | ((TUN_YCS_MODE_MASK & value)<<6);
	if (value & UPD64083_SET_COMP_YCSMODE) 
		*comp_reg = (COMP_YCS_MODE_MASK & value)<<4;
	if (value & UPD64083_SET_SVID_YCSMODE) 
		*svid_reg = (SVID_YCS_MODE_MASK & value)<<2;

	// USE INTERNAL/EXTERNAL Y ADC
	tun_reg = &(config->data->regs[R02]);
	comp_reg = &(config->linein_y_adc[0]);
	svid_reg = &(config->linein_y_adc[1]);

	if (value & UPD64083_TUN_USE_INT_Y_ADC)
		*tun_reg = *tun_reg & ~USE_EXTERNAL_Y_ADC_MASK;
	if (value & UPD64083_TUN_USE_EXT_Y_ADC)
		*tun_reg = (*tun_reg & ~USE_EXTERNAL_Y_ADC_MASK) | USE_EXTERNAL_Y_ADC_MASK;

	if (value & UPD64083_COMP_USE_INT_Y_ADC)
		*comp_reg = 0x00;
	if (value & UPD64083_COMP_USE_EXT_Y_ADC)
		*comp_reg = USE_EXTERNAL_Y_ADC_MASK;

	if (value & UPD64083_SVID_USE_INT_Y_ADC)
		*svid_reg = 0x00;
	if (value & UPD64083_SVID_USE_EXT_Y_ADC)
		*svid_reg = USE_EXTERNAL_Y_ADC_MASK;
}

/* ------------------------------------------------------------------------ */

static int upd64083_set_configure(struct i2c_client *client, u8 command, u32 value)
{
	struct upd64083_config_st *config = i2c_get_clientdata(client);

	switch (command) {
	case UPD64083_DUMMY:
		break;
	case UPD64083_CHANGE_DEF_SET:
		if (value < def_set_max)
		       	return -ERANGE;
		config->def_set_num = value;
		upd64083_set_norm(client);
		break;
	case UPD64083_SET_PARAM1:
		upd64083_set_param1_configure(client,value);
		break;
	default:
		return -ERANGE;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

static int upd64083_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	struct upd64083_config_st *config = i2c_get_clientdata(client);

	switch(cmd) {
	case VIDIOC_S_CTRL:
	{
		struct v4l2_control *ctrl = (struct v4l2_control *)arg;
		u32 arg_body;
		u8 command;
		u32 value;
		int rc;

		if (ctrl->id != UPD640XX_SET_VALUE)
			return -EINVAL;

		arg_body = (u32)ctrl->value;
		v4l_dbg(1, debug, client, //tadachi
			"UPD640XX_SET_VALUE DEBUG: val:%08X\n",
			arg_body);
		command = ((arg_body & UPD640XX_SET_COMMAND_MASK)
			   >> UPD640XX_SET_COMMAND_SHIFT);
		value   = arg_body & UPD640XX_SET_VALUE_MASK;

		rc = upd64083_set_configure(client, command, value);
		if (rc != 0) return  rc;
		v4l_dbg(1, debug, client,
			"UPD640XX_SET_VALUE: com:%01X val:%04X\n",
			command,value);

		write_all_regs(client);  //rewrite all regs
		chg_input(client);    // rewrite chg input
		break;
	}

	case VIDIOC_S_INPUT:
		if (*(int*)arg > UPD640XX_INPUT_TYPE_MAX)
		   return -EINVAL;
		config->current_input_type = *(int*)arg;
		chg_input(client);
		break;

	case VIDIOC_LOG_STATUS:
		upd64083_log_status(client);
		break;

#ifdef CONFIG_VIDEO_ADV_DEBUG
	case VIDIOC_INT_G_REGISTER:
	{
		struct v4l2_register *reg = arg;

		if (reg->i2c_id != I2C_DRIVERID_UPD64083)
			return -EINVAL;
		reg->val = upd64083_read(client, reg->reg & 0xff);
		break;
	}

	case VIDIOC_INT_S_REGISTER:
	{
		struct v4l2_register *reg = arg;
		u8 addr = reg->reg & 0xff;
		u8 val = reg->val & 0xff;

		if (reg->i2c_id != I2C_DRIVERID_UPD64083)
			return -EINVAL;
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
		/* yc mode get from ext. command */
		if (addr == 0x00)
		  //tadachi	state->yc_mode = val;
		upd64083_write(client, addr, val);
		break;
	}
#endif
	
	default:
		break;
	}

	return 0;
}

/* ------------------------------------------------------------------------ */

/* i2c implementation */

static struct i2c_driver i2c_driver;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
static int upd64083_attach(struct i2c_adapter *adapter, int address, int kind)
#else
static int upd64083_attach(struct i2c_adapter *adapter, int address,
			 unsigned short flags, int kind)
#endif
{
	struct i2c_client *client;
	struct upd64083_config_st *config;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
		return 0;

	client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	if (client == NULL) {
		return -ENOMEM;
	}

	memset(client, 0, sizeof(struct i2c_client));
	client->addr = address;
	client->adapter = adapter;
	client->driver = &i2c_driver;
	/*
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
	client->flags = I2C_CLIENT_ALLOW_USE;
#endif*/
	snprintf(client->name, sizeof(client->name) - 1, "uPD64083");

	v4l_info(client, "chip found @ 0x%x (%s)\n", address << 1, adapter->name);
	if (card)
		v4l_info(client, "selected card type %d\n", card);

	config = kmalloc(sizeof(struct upd64083_config_st), GFP_KERNEL);
	if (config == NULL) {
		kfree(client);
		return -ENOMEM;
	}
	config->data = kmalloc(sizeof(struct upd64083_regdata_st), GFP_KERNEL);
	if (config->data == NULL) {
		kfree(config);
		kfree(client);
		return -ENOMEM;
	}
	i2c_set_clientdata(client, config);
	config->def_set_num = 0;
        upd64083_set_norm(client);

	// write init values
	write_all_regs(client);

	i2c_attach_client(client);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	MOD_INC_USE_COUNT;
#endif
	return 0;
}

static int upd64083_probe(struct i2c_adapter *adapter)
{
#ifdef I2C_CLASS_TV_ANALOG
	if (adapter->class & I2C_CLASS_TV_ANALOG)
#else
	if (adapter->id == I2C_HW_B_BT848)
#endif
		return i2c_probe(adapter, &addr_data, upd64083_attach);
	return 0;
}

static int upd64083_detach(struct i2c_client *client)
{
	int err;

	err = i2c_detach_client(client);
	if (err)
	       	return err;

	kfree(client);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	MOD_DEC_USE_COUNT;
#endif
	return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
static struct i2c_driver i2c_driver = {
	/*
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) && LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
	.owner = THIS_MODULE,
#endif
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
	.name = "uPD64083 driver",
	.flags = I2C_DF_NOTIFY,
#else*/
	.driver = {
		.name = "upd64083",
	},
/*#endif*/
	.id = I2C_DRIVERID_UPD64083,
	.attach_adapter = upd64083_probe,
	.detach_client  = upd64083_detach,
	.command = upd64083_command,
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
EXPORT_NO_SYMBOLS;
#endif

static int __init upd64083_init_module(void)
{
	return i2c_add_driver(&i2c_driver);
}

static void __exit upd64083_exit_module(void)
{
	i2c_del_driver(&i2c_driver);
}

module_init(upd64083_init_module);
module_exit(upd64083_exit_module);
