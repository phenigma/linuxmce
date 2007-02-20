/* 
 * upd64031A - NEC Electronics Ghost Reducer for NTSC in Japan driver version 0.0.1
 *
 * 2003 by T.Adachi (tadachi@tadachi-net.com)
 */

#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>

#include <linux/i2c.h>
#include <linux/videodev2.h>
#include <media/v4l2-common.h>

#include "upd640xx.h"

// --------------------- read registers functions define -----------------------
// SA00h reg ***
#define R_KILLER_FLAG_MASK            0x80    // bit 7, 0:normal, 1: croma disabled
#define R_HANTING_FLAG_MASK           0x40    // bit 6, 0:normal, 1: hanting
#define R_GHOST_REDUCE_PROC_MASK      0x20    // bit 5, 0:stopping, 1: active
#define R_GHOST_SIGNAL_DETECTED_MASK  0x10    // bit 4, 0:GCR not detected, 1: GCR detected
#define R_DVG_FLAG_MASK               0x08    // bit 3, 0:normal action, 1:filter coefficient oscillation
#define R_INTERNAL_INTEGRATION_TIMES_MASK    0x07    // bit 2:0
#define R_WP2I_MASK            0x20    // WP2 port data
#define R_WP3I_MASK            0x10    // WP3 port data
#define R_HARD_VER_MASK        0x0C    // hardware version
#define R_SOFT_VER_MASK        0x03    // software version
// -----------------------------------------------------------------------------

#define UPD64031A_INPUT_TYPE_MAX  1

/* bit masks */
#define GR_MODE_MASK                 0xc0
#define PRE_GHOST_FILT_RANGE_MASK    0x10
#define ADC_CLOCK_DELAY_MASK         0xc0
#define DIRECT_3DYCS_CONNECT_MASK    0xc0
#define COMPOSITE_SYNC_CIRCUIT_MASK  0x80
#define VERTICAL_SYNC_CIRCUIT_MASK   0x20
#define CH_CHG_CTRL_MASK             0x10
#define DIGITAL_CLAMP_AMP_GAIN_MASK  0x38
#define DIGITAL_CLAMP_LOOP_GAIN_MASK 0x07

#define GR_MODE_ON         0x00
#define GR_MODE_THROUGH    0x80
#define GR_MODE_OFF        0x40

#define USE_INTERNAL_COMPOSITE_SYNC_DIGITAL_SEPARATION_CIRCUIT 0x00 // SA05 bit 7
#define USE_EXTERNAL_COMPOSITE_SYNC_DIGITAL_SEPARATION_CIRCUIT 0x80 // SA05 bit 7
#define USE_INTERNAL_VERTICAL_SYNC_DIGITAL_SEPARATION_CIRCUIT  0x00 // SA05 bit 5
#define USE_EXTERNAL_VERTICAL_SYNC_DIGITAL_SEPARATION_CIRCUIT  0x80 // SA05 bit 5

#define DIRECT_3DYCS_CONNECT_DISABLE                   0x00    // SA08 bit 7:6
#define DIRECT_3DYCS_CONNECT_ENABLE_INP_IS_COMPOSITE   0x80    // SA08 bit 7:6
#define DIRECT_3DYCS_CONNECT_ENABLE_INP_IS_SVIDEO      0xc0    // SA08 bit 7:6

// -----------------------------------------------------------------------------

MODULE_DESCRIPTION("uPD64031A Driver");
MODULE_AUTHOR("T. Adachi <tadachi@tadachi-net.com>, Takeru KOMORIYA <komoriya@paken.org>");
MODULE_LICENSE("GPL");

static int debug = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
module_param(debug, int, 0644);
#else
MODULE_PARM(debug, "i");
#endif

MODULE_PARM_DESC(debug, "Debug level (0-1)");

static unsigned short normal_i2c[] = { 0x24 >> 1, 0x26 >> 1, I2C_CLIENT_END };

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };
#endif

I2C_CLIENT_INSMOD;

enum {
        R00 = 0, R01, R02, R03, R04,
	R05, R06, R07, R08, R09,
	R0A, R0B, R0C, R0D, R0E, R0F,
	/* unused registers
	 R10, R11, R12, R13, R14,
	 R15, R16, R17,
	 */
        TOT_REGS
};

struct upd64031a_regdata_st {
	char  *name;
	u8    regs[TOT_REGS];
};

struct upd64031a_config_st {
	unsigned int def_set_num;
	u8 current_input_type;
	struct upd64031a_regdata_st *data;

	u8 linein_gr_mode;
	u8 linein_3dycs_connect;
};

static struct upd64031a_regdata_st upd64031a_norms[] = {
	{
		.name = "3DYCS_DIG_CONNECT_SETTING",
		.regs = {
			0x00, 0xb8, 0x48, 0xd2, 0xe6,
			0x03, 0x10, 0x0b, 0xaf, 0x7f,
			0x00, 0x00, 0x1d, 0x5e, 0x00,
			0xd0
		}
	}
};

static int def_set_max = ARRAY_SIZE(upd64031a_norms);
 
#ifndef I2C_DRIVERID_UPD64031A
#define I2C_DRIVERID_UPD64031A I2C_DRIVERID_EXP2
#endif

/* ------------------------------------------------------------------------ */

static u8 upd64031a_read(struct i2c_client *client, u8 reg)
{
	u8 buf[2];

	if (reg >= sizeof(buf))
		return 0xff;
	i2c_master_recv(client, buf, 2);
	return buf[reg];
}

static int upd64031a_write(struct i2c_client *client, u8 reg, u8 val)
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

static int write_all_regs(struct i2c_client *client)
{
        struct upd64031a_config_st *config = i2c_get_clientdata(client);
	int i;

	v4l_dbg(1, debug, client, "writing all registers\n");
	for (i = 0; i < TOT_REGS; i++) {
		if (upd64031a_write(client, i, config->data->regs[i]) != 0)
		       	return -1;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

// change channel
static int chg_channel(struct i2c_client *client)
{
        struct upd64031a_config_st *config = i2c_get_clientdata(client);
	u8 reg;
	int rc;

	v4l_dbg(1, debug, client, "change channel\n");

	reg = config->data->regs[R00];
        rc = upd64031a_write(client, R00, reg | CH_CHG_CTRL_MASK);
	if (rc != 0)
	       	return rc;
        return upd64031a_write(client, R00, reg & (~CH_CHG_CTRL_MASK));
}

/* ------------------------------------------------------------------------ */

// change input (type)
//   0:tuner 1:line-in(composite/s-video)
static int chg_input(struct i2c_client *client)
{
	static const char *input_name[] = {"Tuner", "Line-in (Composite/S-Video)"};
        struct upd64031a_config_st *config = i2c_get_clientdata(client);

	u8 r00, r08;
	int rc;

	if (config->current_input_type > UPD64031A_INPUT_TYPE_MAX)
	       	return -ERANGE;

	if (config->current_input_type == 0)  {
		// tuner
		r00 = config->data->regs[R00];
		r08 = config->data->regs[R08];
	} else {
		// line-in
		r00 = (config->data->regs[R00] & ~GR_MODE_MASK) |
			config->linein_gr_mode;
		r08 = (config->data->regs[R08] & ~DIRECT_3DYCS_CONNECT_MASK) |
			config->linein_3dycs_connect;
	}

	v4l_dbg(1, debug, client, "change input to %s\n",
			input_name[config->current_input_type]);

	rc = upd64031a_write(client,R00,r00);
	if (rc != 0)
	       	return rc;
	return upd64031a_write(client,R08,r08);
}

/* ------------------------------------------------------------------------ */

static int upd64031a_set_norm(struct i2c_client *client)
{
        struct upd64031a_config_st *config = i2c_get_clientdata(client);
	int i;

	if (config->def_set_num > def_set_max)
	   return -ERANGE;

	for (i = 0; i < TOT_REGS; i++) {
		config->data->regs[i] = upd64031a_norms[config->def_set_num].regs[i];
	}

	config->current_input_type = 0;
	config->linein_gr_mode = (config->data->regs[R00] & GR_MODE_MASK);
	config->linein_3dycs_connect =
		DIRECT_3DYCS_CONNECT_ENABLE_INP_IS_SVIDEO;
	v4l_dbg(1, debug, client, "set norm to [%d]\n", config->def_set_num);
	return 0;
}

/* ------------------------------------------------------------------------ */

static void upd64031a_set_param1_configure(struct i2c_client *client, u32 value)
{
	struct upd64031a_config_st *config = i2c_get_clientdata(client);
	u8 *tun_reg, *lin_reg;

	// GR MODE -----
	tun_reg = &config->data->regs[R00];
	lin_reg = &config->linein_gr_mode;
	if (value & UPD64031A_TUN_GRON)
		*tun_reg = (*tun_reg & ~GR_MODE_MASK) | GR_MODE_ON;
	if (value & UPD64031A_TUN_GROFF) {
		if (value & UPD64031A_TUN_GR_THROUGH) {
			*tun_reg = (*tun_reg & ~GR_MODE_MASK) | GR_MODE_OFF |GR_MODE_THROUGH;
		} else {
			*tun_reg = (*tun_reg & ~GR_MODE_MASK) | GR_MODE_OFF;
		}
	}

	if (value & UPD64031A_LIN_GRON)
		*lin_reg = (*lin_reg & ~GR_MODE_MASK) | GR_MODE_ON;
	if (value & UPD64031A_LIN_GROFF) {
		if (value & UPD64031A_LIN_GR_THROUGH) {
			*lin_reg = (*lin_reg & ~GR_MODE_MASK) | GR_MODE_OFF | GR_MODE_THROUGH;
		} else {
			*lin_reg = (*lin_reg & ~GR_MODE_MASK) | GR_MODE_OFF;
		}
	}

	// DIRECT 3DYCS CONNECTION -----
	tun_reg = &config->data->regs[R08];
	lin_reg = &config->linein_3dycs_connect;
	if (value & UPD64031A_TUN_3DYCS_DISABLE)
		*tun_reg = (*tun_reg & ~DIRECT_3DYCS_CONNECT_MASK) | DIRECT_3DYCS_CONNECT_DISABLE;
	if (value & UPD64031A_TUN_3DYCS_COMPOSITE)
		*tun_reg = (*tun_reg & ~DIRECT_3DYCS_CONNECT_MASK) | DIRECT_3DYCS_CONNECT_ENABLE_INP_IS_COMPOSITE;
	if (value & UPD64031A_TUN_3DYCS_SVIDEO)
		*tun_reg = (*tun_reg & ~DIRECT_3DYCS_CONNECT_MASK) | DIRECT_3DYCS_CONNECT_ENABLE_INP_IS_SVIDEO;

	if (value & UPD64031A_LIN_3DYCS_DISABLE)
		*lin_reg = (*lin_reg & ~DIRECT_3DYCS_CONNECT_MASK) | DIRECT_3DYCS_CONNECT_DISABLE;
	if (value & UPD64031A_LIN_3DYCS_COMPOSITE)
		*lin_reg = (*lin_reg & ~DIRECT_3DYCS_CONNECT_MASK) | DIRECT_3DYCS_CONNECT_ENABLE_INP_IS_COMPOSITE;
	if (value & UPD64031A_LIN_3DYCS_SVIDEO)
		*lin_reg = (*lin_reg & ~DIRECT_3DYCS_CONNECT_MASK) | DIRECT_3DYCS_CONNECT_ENABLE_INP_IS_SVIDEO;

	// SYNC CIRCUIT -----
	tun_reg = &(config->data->regs[R05]);
	if (value & UPD64031A_COMPOSITE_EXTERNAL)
		*tun_reg = (*lin_reg & ~USE_EXTERNAL_COMPOSITE_SYNC_DIGITAL_SEPARATION_CIRCUIT) |
			USE_EXTERNAL_COMPOSITE_SYNC_DIGITAL_SEPARATION_CIRCUIT;
	if (value & UPD64031A_VERTICAL_EXTERNAL)
		*tun_reg = (*lin_reg & ~USE_EXTERNAL_VERTICAL_SYNC_DIGITAL_SEPARATION_CIRCUIT) |
			USE_EXTERNAL_VERTICAL_SYNC_DIGITAL_SEPARATION_CIRCUIT;
}

/* ------------------------------------------------------------------------ */

static int upd64031a_set_configure(struct i2c_client *client, u8 command, u32 value)
{
	struct upd64031a_config_st *config = i2c_get_clientdata(client);

	switch (command) {
	case UPD64031A_DUMMY:
		break;
	case UPD64031A_CHANGE_DEF_SET:
		if (value < def_set_max)
		       	return -ERANGE;
		config->def_set_num = value;
		upd64031a_set_norm(client);
		break;
	case UPD64031A_SET_PARAM1:
		upd64031a_set_param1_configure(client,value);
		break;
	default:
		return -ERANGE;
	}
	return 0;
}

/* ------------------------------------------------------------------------ */

static int upd64031a_command(struct i2c_client *client, unsigned int cmd, void *arg)
{
	struct upd64031a_config_st *config = i2c_get_clientdata(client);

	switch (cmd) {
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

		rc = upd64031a_set_configure(client, command, value);
		if (rc != 0) return  rc;
		v4l_dbg(1, debug, client,
			"UPD640XX_SET_VALUE: com:%01X val:%04X\n",
			command,value);

		write_all_regs(client);  //rewrite all regs
		chg_input(client);    // rewrite chg input
		chg_channel(client);  // rewrite chg channel (if tuner is selected)
		break;
	}

	case VIDIOC_S_FREQUENCY:
		chg_channel(client);
		break;

	case VIDIOC_S_INPUT:
		if (*(int*)arg > UPD640XX_INPUT_TYPE_MAX)
			return -EINVAL;
		config->current_input_type = *(int*)arg;
		if (config->current_input_type > UPD64031A_INPUT_TYPE_MAX)
			config->current_input_type = UPD64031A_INPUT_TYPE_MAX;
		chg_input(client);
		break;

	case VIDIOC_LOG_STATUS:
		v4l_info(client, "Status: SA00=0x%02x SA01=0x%02x\n",
			upd64031a_read(client, 0), upd64031a_read(client, 1));
		break;

#ifdef CONFIG_VIDEO_ADV_DEBUG
	case VIDIOC_INT_G_REGISTER:
	{
		struct v4l2_register *reg = arg;

		if (reg->i2c_id != I2C_DRIVERID_UPD64031A)
			return -EINVAL;
		reg->val = upd64031a_read(client, reg->reg & 0xff);
		break;
	}

	case VIDIOC_INT_S_REGISTER:
	{
		struct v4l2_register *reg = arg;
		u8 addr = reg->reg & 0xff;
		u8 val = reg->val & 0xff;

		if (reg->i2c_id != I2C_DRIVERID_UPD64031A)
			return -EINVAL;
		if (!capable(CAP_SYS_ADMIN))
			return -EPERM;
/* --tadachi
		// gr mode get from ext. command
		if (addr == 0x00)
			state->gr_mode = val;
		else if (addr == 0x02)
			state->gr_level = val;
*/
		upd64031a_write(client, addr, val);
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
static int upd64031a_attach(struct i2c_adapter *adapter, int address, int kind)
#else
static int upd64031a_attach(struct i2c_adapter *adapter, int address,
			 unsigned short flags, int kind)
#endif
{
	struct i2c_client *client;
        struct upd64031a_config_st *config;

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
	snprintf(client->name, sizeof(client->name) - 1, "uPD64031A");

	v4l_info(client, "chip found @ 0x%x (%s)\n", address << 1, adapter->name);

	config = kmalloc(sizeof(struct upd64031a_config_st), GFP_KERNEL);
	if (config == NULL) {
		kfree(client);
		return -ENOMEM;
	}
	config->data = kmalloc(sizeof(struct upd64031a_regdata_st), GFP_KERNEL);
	if (config->data == NULL) {
		kfree(config);
		kfree(client);
		return -ENOMEM;
	}
	i2c_set_clientdata(client, config);
	config->def_set_num = 0;
        upd64031a_set_norm(client);

	// write init values
	write_all_regs(client);

	i2c_attach_client(client);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
	MOD_INC_USE_COUNT;
#endif
	return 0;
}

static int upd64031a_probe(struct i2c_adapter *adapter)
{
#ifdef I2C_CLASS_TV_ANALOG
	if (adapter->class & I2C_CLASS_TV_ANALOG)
#else
	if (adapter->id == I2C_HW_B_BT848)
#endif
		return i2c_probe(adapter, &addr_data, upd64031a_attach);
	return 0;
}

static int upd64031a_detach(struct i2c_client *client)
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
	.name = "uPD64031A driver",
	.flags = I2C_DF_NOTIFY,
#else*/
	.driver = {
		.name = "upd64031a",
	},
/*#endif*/
	.id = I2C_DRIVERID_UPD64031A,
	.attach_adapter = upd64031a_probe,
	.detach_client  = upd64031a_detach,
	.command = upd64031a_command,
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
EXPORT_NO_SYMBOLS;
#endif

static int __init upd64031a_init_module(void)
{
	return i2c_add_driver(&i2c_driver);
}

static void __exit upd64031a_exit_module(void)
{
	i2c_del_driver(&i2c_driver);
}

module_init(upd64031a_init_module);
module_exit(upd64031a_exit_module);

