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

MODULE_DESCRIPTION("wm8775 driver");
MODULE_AUTHOR("Ulf Eklund");
MODULE_LICENSE("GPL");

#include "compat.h"

#ifndef LINUX26
#include <linux/wrapper.h>
#endif

#include <linux/types.h>
#include <linux/ioctl.h>
#include <asm/uaccess.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#ifndef I2C_DRIVERID_WM8775
// Using temporary hack for missing I2C driver-ID for WM8775
#define I2C_DRIVERID_WM8775 I2C_DRIVERID_EXP1
#endif

#include "wm8775.h"

static int debug = 0;
MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug level (0-1)");
static int mixer = 0xf;
MODULE_PARM(mixer, "i");
MODULE_PARM_DESC(mixer, "Mixer select (0-f)");


#define MOD_NAME "wm8775"

#define LOG_ERR(fmt, args...)  printk(KERN_ERR  MOD_NAME ": " fmt "\n" ,##args)
#define LOG_INFO(fmt, args...) printk(KERN_ERR MOD_NAME ": " fmt "\n" ,##args)
#define IE(fmt, args...)       printk(KERN_ERR  MOD_NAME ": Internal Error: " fmt "\n" ,##args)

#define DEB(num, fmt, args...) \
  do { if (debug >= num) LOG_INFO(fmt ,##args); } while (0)


/* ----------------------------------------------------------------------- */

#define ELEMENTS(a) (sizeof((a))/sizeof(*(a)))

typedef enum {
  R23 = 0, R7, R11, R12, R13, R14, R15, R16, R17, R18, R19, R20, R21
} wm8775_reg_index;

typedef struct { char *name; __u16 offset; } reg_spec;

static reg_spec wm8775_reg_spec[] = {
  { "R23", 23<<9 },
  { "R7",  07<<9 },
  { "R11", 11<<9 },
  { "R12", 12<<9 },
  { "R13", 13<<9 },
  { "R14", 14<<9 },
  { "R15", 15<<9 },
  { "R16", 16<<9 },
  { "R17", 17<<9 },
  { "R18", 18<<9 },
  { "R19", 19<<9 },
  { "R20", 20<<9 },
  { "R21", 21<<9 },
};

static char *reg_name(int i) {
  if(i<0 || i>ELEMENTS(wm8775_reg_spec)) {
    IE("Register index out of range %d", i);
  }
  return wm8775_reg_spec[i].name;
}

static __u16 reg_offset(int i) {
  if(i<0 || i>ELEMENTS(wm8775_reg_spec)) {
    IE("Register index out of range %d", i);
  }
  return wm8775_reg_spec[i].offset;
}

typedef struct {
  __u16 reg[ELEMENTS(wm8775_reg_spec)];
} wm8775_reg_set;

typedef struct {
  __u16 value;
  char *name;
} value_map;

typedef struct {
  __u16              reg_index;
  __u16              bit_offset;
  __u16              bits;
  __u16              reset_value;
  char              *name;
  value_map         *map;
} setting_spec;

static setting_spec wm8775_setting_spec[] = {
  /*RESET,  */ { R23, 0, 1, 0x01, "Reset",    },  //OK
  /*TOD,    */ { R7,  3, 1, 0x00, "TOD",      0}, //OK
  /*ADCHPD, */ { R11, 8, 1, 0x00, "ADCHPD",   0}, //OK
  /*ADCMCLK,*/ { R11, 6, 1, 0x00, "ADCMCLK",  0}, //OK
  /*ADCWL,  */ { R11, 4, 2, 0x02, "ADCWL",    0}, //OK
  /*ADCBCP, */ { R11, 3, 1, 0x00, "ADCBCP",   0}, //OK
  /*ADCLRP, */ { R11, 2, 1, 0x00, "ADCLRP",   0}, //OK
  /*ADCFMT, */ { R11, 0, 2, 0x02, "ADCFMT",   0}, //OK
  /*ADCMS,  */ { R12, 8, 1, 0x00, "ADCMS",    0}, //OK
  /*ADCOSR, */ { R12, 3, 1, 0x00, "ADCOSR",   0}, //OK
  /*ADCRATE,*/ { R12, 0, 3, 0x02, "ADCRATE",  0}, //OK
  /*AINPD,  */ { R13, 6, 1, 0x00, "AINPD",    0}, //OK 
  /*ADCPD,  */ { R13, 1, 1, 0x00, "ADCPD",    0}, //OK
  /*PWDN,   */ { R13, 0, 1, 0x00, "PWDN",     0}, //OK
  /*ZCLA,   */ { R14, 8, 1, 0x00, "ZCLA",     0}, //OK
  /*LAG,    */ { R14, 0, 8, 0xCF, "LAG",      0}, //OK
  /*ZCRA,   */ { R15, 8, 1, 0x00, "ZCRA",     0}, //OK
  /*RAG,    */ { R15, 0, 8, 0xCF, "RAG",      0}, //OK
  /*LCSEL,  */ { R16, 7, 2, 0x00, "LCSEL",    0}, //OK 
  /*MAXGAIN,*/ { R16, 4, 3, 0x07, "MAXGAIN",  0}, //OK
  /*LCT,    */ { R16, 0, 4, 0x0B, "LCT",      0}, //OK
  /*LCEN,   */ { R17, 8, 1, 0x00, "LCEN",     0}, //OK
  /*ALCZC,  */ { R17, 7, 1, 0x00, "ALCZC",    0}, //OK 
  /*HLD,    */ { R17, 0, 4, 0x00, "HLD",      0}, //OK
  /*DCY,    */ { R18, 4, 4, 0x03, "DCY",      0}, //OK
  /*ATK,    */ { R18, 0, 4, 0x02, "ATK",      0}, //OK
  /*NGTH,   */ { R19, 2, 3, 0x00, "NGTH",     0}, //OK
  /*NGAT,   */ { R19, 0, 1, 0x00, "NGAT",     0}, //OK
  /*TRANWIN,*/ { R20, 4, 3, 0x02, "TRANWIN",  0}, // OK
  /*MAXATTEN*/ { R20, 0, 4, 0x06, "MAXATTEN", 0}, // OK
  /*LRBOTH, */ { R21, 8, 1, 0x00, "LRBOTH",   0}, // OK
  /*MUTELA, */ { R21, 7, 1, 0x00, "MUTELA",   0}, // OK
  /*MUTERA, */ { R21, 6, 1, 0x00, "MUTERA",   0}, // OK
  /*AMX,    */ { R21, 0, 4, 0x01, "AMX",      0}, // OK
};

static int activate(struct i2c_client *client, wm8775_reg_set *config) { 
  int i;
  wm8775_reg_set *s;
  
  s=(wm8775_reg_set*)i2c_get_clientdata(client);
  if(!s) { IE("NULL client state pointer returned. (%p)", s); return -1 ; }
  
  for(i=0; i<ELEMENTS(config->reg); i++) {
    DEB(0, "%s register %d (%s), config: %02x%02x active: %02x%02x.", 
	s->reg[i]!=config->reg[i]?"Updating":"Skipping", i, reg_name(i), 
	s->reg[i]>>8,      s->reg[i]&0xFF, 
	config->reg[i]>>8, config->reg[i]&0xFF); 
    if (s->reg[i]!=config->reg[i]) {
      __u8 buffer[2];
      buffer[0]=config->reg[i]>>8;
      buffer[1]=config->reg[i]&0xff;
      if (2 != i2c_master_send(client, buffer, 2)) {
	LOG_ERR("Failed to update register %d (%s) with %02x%02x.", i, reg_name(i), buffer[0], buffer[1]); 
      }
      s->reg[i]=config->reg[i];
    }
  }
  s->reg[R23]|=0x0001; // Clear reset flag.
  return 0;
}

static __u16 set_value(wm8775_reg_set *dst, __u16 setting_id, __u16 val) { 
  return dst->reg[wm8775_setting_spec[setting_id].reg_index]=(dst->reg[wm8775_setting_spec[setting_id].reg_index]&
	  ~(((1<<wm8775_setting_spec[setting_id].bits)-1)<<wm8775_setting_spec[setting_id].bit_offset))|(val<<wm8775_setting_spec[setting_id].bit_offset);
}
static __u16 get_value(wm8775_reg_set *src, __u16 setting_id) {
  return (src->reg[wm8775_setting_spec[setting_id].reg_index]>>wm8775_setting_spec[setting_id].bit_offset)&((1<<(wm8775_setting_spec[setting_id].bits))-1);
}
#define valid_setting(val, setting_id) (val<=(1<<(wm8775_setting_spec[setting_id].bits)))
#define setting_name(setting_id)       (wm8775_setting_spec[setting_id].name)
#define reset_value(setting_id)        (wm8775_setting_spec[setting_id].reset_value)
#define reg_index(setting_id)          (wm8775_setting_spec[setting_id].reg_index)
#define bit_offset(setting_id)         (wm8775_setting_spec[setting_id].bit_offset)
#define bits(setting_id)               (wm8775_setting_spec[setting_id].bits)

static void set_default_config (wm8775_reg_set *t) {
  int i;
  
  for(i=0; i<ELEMENTS(t->reg); i++) {
    t->reg[i]=reg_offset(i);
    DEB(1, "Offset reset.reg[%d]=%04x", i, t->reg[i]);
  }

  for(i=0; i<ELEMENTS(wm8775_setting_spec); i++) {
    set_value(t, i, reset_value(i));
    DEB(1, "Updating reg[%s]=%04x added %s with default %d",
	reg_name(reg_index(i)), t->reg[reg_index(i)], setting_name(i), reset_value(i));
  }

  for(i=0; i<ELEMENTS(t->reg); i++) {
    DEB(1, "Final reset.reg[%d]=%04x", i, t->reg[i]);
  }
  
  return ;
}

static int configure (struct i2c_client *client,
		      config_request     *conf,
		      int                conf_len) {
  wm8775_reg_set *s;
  wm8775_reg_set c;
  int conf_i;
  int i;
  
  s=(wm8775_reg_set*)i2c_get_clientdata(client);
  if(!s) { IE("NULL client state pointer returned. (%p)", s); return -EINVAL ; }
  
  // Copy current state
  for(i=0; i<ELEMENTS(c.reg); i++) c.reg[i]=s->reg[i];
  
  for(conf_i=0; conf_i<conf_len; conf_i++) {
    
    if(conf[conf_i].setting_id>=ELEMENTS(wm8775_setting_spec)) { 
      LOG_ERR("configuration entry %d, config item id %d not valid", conf_i, conf[conf_i].setting_id);
      return -EINVAL;
    }
    
    if (conf[conf_i].setting_id==RESET) {
      set_default_config(&c);
      c.reg[R23]&=~0x0001; // Clear reset flag.
      continue;
    }
    
    if(!valid_setting(conf[conf_i].value, conf[conf_i].setting_id)) {
      LOG_INFO("configuration entry %d, %s, value %d out of range",conf_i, setting_name(conf[conf_i].setting_id), conf[conf_i].value);
      return -EINVAL;
    }
    // FIXME VALUE MAP HANDLING
    
    {
      u16 old; // FIXME only debub
      old=get_value(&c, conf[conf_i].setting_id);
      set_value(&c, conf[conf_i].setting_id,  conf[conf_i].value);
      DEB(0, "Setting register %s (%d) to %d, %s.%d..%d, old value %02x",
	  setting_name(conf[conf_i].setting_id), 
	  conf[conf_i].setting_id,
	  conf[conf_i].value, 
	  reg_name(reg_index(conf[conf_i].setting_id)), 
	  bit_offset(conf[conf_i].setting_id),
	  bit_offset(conf[conf_i].setting_id)+bits(conf[conf_i].setting_id)-1,
	  old);
    }
  }
  if (activate(client, &c)) return -EINVAL;
  return 0;
}



static int log_state (struct i2c_client *client) {
  wm8775_reg_set *s;
  int setting_i;
  int i;
  
  s=(wm8775_reg_set*)i2c_get_clientdata(client);
  if(!s) { IE("NULL client state pointer returned. (%p)", s); return -EINVAL ; }
  
  for(i=0; i<ELEMENTS(s->reg); i++) 
    LOG_INFO("Register %s=%04x", reg_name(i), s->reg[i]);
  
  for(setting_i=0; setting_i<ELEMENTS(wm8775_setting_spec); setting_i++) {
    // FIXME add value map handling
    LOG_INFO("Register %s=%d", setting_name(setting_i), get_value(s, setting_i));
  }
  return 0;
}

static int get_configuration (struct i2c_client *client,
			      config_request    *config,
			      int                no_entries) {
  wm8775_reg_set  *s;
  int setting_i;

  s=(wm8775_reg_set*)i2c_get_clientdata(client);
  if(!s) { IE("NULL client state pointer returned. (%p)", s); return -EINVAL ; }
  
  if (no_entries<ELEMENTS(wm8775_setting_spec)) {
    LOG_INFO("insufficient space to store config. (%d)", no_entries);
    return -EINVAL;
  }
  
  for(setting_i=0; setting_i<ELEMENTS(wm8775_setting_spec); setting_i++) {
    config->setting_id=setting_i;
    config->value=get_value(s, setting_i);
  }
  
  return 0;
}

static int command (struct i2c_client *client,
		    unsigned int       cmd,
		    void              *arg) {  
  wm8775_ioctl *i = arg;
  
  switch (cmd) {
  case WM8775_CONFIGURE:
    if(!i) { LOG_INFO("Invalid ioctl argument pointer. (%p)", i); return -EINVAL ; }
    return configure(client, i->list, i->list_length);
  case WM8775_LOG_STATE:
    return log_state(client);
  case WM8775_GET_STATE:
    if(!i) { LOG_INFO("Invalid ioctl argument pointer. (%p)", i); return -EINVAL ; }
    return get_configuration(client, i->list, i->list_length);
  default:
    return -EINVAL;
  }
  return 0;
}

/* ----------------------------------------------------------------------- */


/* i2c implementation */
#ifndef NEW_I2C
  /* pre i2c-2.8.0 */

static void inc_use (struct i2c_client *client) {
  #ifdef MODULE
	MOD_INC_USE_COUNT;
  #endif
}

static void dec_use (struct i2c_client *client) {
  #ifdef MODULE
	MOD_DEC_USE_COUNT;
  #endif
}

#else
	/* i2c-2.8.0 and later */
#endif

#define I2C_WM8775_ADDRESS  0x36

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */
static unsigned short normal_i2c[] = { I2C_WM8775_ADDRESS >> 1, I2C_CLIENT_END };
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };

I2C_CLIENT_INSMOD;

struct i2c_driver i2c_driver;

static int detect_client (struct i2c_adapter *adapter,
			  int                 address,
#ifndef LINUX26
			  unsigned short      flags,
#endif
			  int                 kind) {
  struct i2c_client *client;
  
  DEB(1, "detecting client on address 0x%x",
      address << 1);
  
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
  snprintf(client->name, sizeof(client->name), MOD_NAME);
  
  {
    wm8775_reg_set *state;
    config_request xp_config[] = { 
      // I2C Scan values in XP, some values are guessed due to broken scan...
      {RESET,    0},
      //   R7 0x000
      //  R11 0x021 ADCFMT=1
      {ADCFMT,   1},
      //  R12 0x102 ADCMS=1 
      {ADCMS,    1},
      //  R13 0x000
      //  R14 0x1d4 ZCLA=1 LAG=0xd4
      //      {ZCLA,    1},
      //      {ZCLA,    0xd4},
      //  R15 0x1d4 ZCRA=1 RAG=0xd4
      //      {ZCRA,    1},
      //      {ZCRA,    0xd4},
      //  R16 0x1eb LCSEL=3 MAXGAIN=6
      //      {LCSEL,    3},
      //      {MAXGAIN,  6},
      //  R17 0x188 LCEN=1 HLD=8 ALCZC=1
      //      {LCEN,     1},
      //      {HLD,      8},
      //      {ALCZC,    1},
      //  R18 0x082 DCY=8 
      {DCY,      8},
      //  R19 0x001 NGAT=1
      {NGAT,     1},
      //  R20 0x07a TRAWIN=7 MAXATTEN=11
      //    {TRANWIN,  7},
      //    {MAXATTEN, 11},
      //  R21 0x102 AMX=2 LRBOTH=1
      {LRBOTH,   1},
      {AMX,      mixer}
    };
    state = kmalloc(sizeof(*state), GFP_KERNEL);
    if (!state) { kfree(client); return -ENOMEM; }
    i2c_set_clientdata(client, state); 
    set_default_config(state);
    configure(client, xp_config, ELEMENTS(xp_config));
  }
  
  i2c_attach_client(client);
  
  return 0;
}

static int attach_adapter (struct i2c_adapter *adapter) {
  DEB(1, "starting probe for adapter %s (0x%x)",
      adapter->name, adapter->id);
  return i2c_probe(adapter, &addr_data, &detect_client);
  return 0;
}

static int detach_client (struct i2c_client *client) {
  wm8775_reg_set *state = i2c_get_clientdata(client);
  int err;
  
  err = i2c_detach_client(client);
  if (err) {
    return err;
  }
  
  kfree(state);
  kfree(client);
  return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
struct i2c_driver i2c_driver = {
  .name = MOD_NAME,

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
#endif
};

#ifndef LINUX26
EXPORT_NO_SYMBOLS;
#endif

static int __init minit (void) {
  DEB(1,"Init");
  return i2c_add_driver(&i2c_driver);
}

static void __exit mexit (void) {
  i2c_del_driver(&i2c_driver);
}

module_init(minit);
module_exit(mexit);
