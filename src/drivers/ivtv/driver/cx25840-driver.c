/* 
 * cx25840 - driver version 0.0.1
 *
 * Copyright (C) 2004 Ulf Eklund
 *
 * Based on the saa7115 driver and on the first verison of Chris Kennedys
 * cx25840 driver.
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
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/videodev.h>
#include <asm/uaccess.h>
#include <linux/video_decoder.h>
#include <linux/crypto.h>

#include "ivtv.h"
#include "cx25840.h"
#include "compat.h"

#include "cx25840-settings.h"
#include "cx25840-registers.h"

#ifndef LINUX26
#include <linux/wrapper.h>
#endif

MODULE_DESCRIPTION("CX25840 Broadcast audio and video decoder driver");
MODULE_AUTHOR("Ulf Eklund");
MODULE_LICENSE("GPL");

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define IVTV_MAX_CARDS 8

#ifndef I2C_DRIVERID_CX25840
// Using temporary hack for missing I2C driver-ID for CX25840
#define I2C_DRIVERID_CX25840 I2C_DRIVERID_EXP3
#endif

static int debug = 0;
MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug level (0-1)");

static char *fw_file_name = NULL;
MODULE_PARM(fw_file_name, "s");
MODULE_PARM_DESC(fw_file_name, "Full file path to fw image");

static int i2c_enable[IVTV_MAX_CARDS] = { 1, 1, 1, 1, 1, 1, 1, 1 };
MODULE_PARM(i2c_enable, "1-" __stringify(IVTV_MAX_CARDS) "i");
MODULE_PARM_DESC(i2c_enable, "which 150/350 cards to activate the cx25840, conflicts with saa7127 or pvr350");

static int no_black_magic = 0;
MODULE_PARM(no_black_magic, "i");
MODULE_PARM_DESC(no_black_magic, "Don't poke bits at 0x43 to power up tuner. Use for PG600!");

static int cardtype = 0;
MODULE_PARM(cardtype, "i");
MODULE_PARM_DESC(cardtype, "Card type: 0=pvr150, 1=pvr500, 2=pg600");

static int audiofmt = 0xf;
MODULE_PARM(audiofmt, "i");
MODULE_PARM_DESC(audiofmt, "Audio Format: default=autodetect, 1=NTSC,...0xE");

#define MOD_NAME "cx25840"

#define ERR(fmt, args...)  printk(KERN_ERR  MOD_NAME ": " fmt "\n" ,##args)
#define WARN(fmt, args...) printk(KERN_WARN MOD_NAME ": " fmt "\n" ,##args)
#define INFO(fmt, args...) printk(KERN_INFO MOD_NAME ": " fmt "\n" ,##args)
#define IE(fmt, args...)   printk(KERN_ERR  MOD_NAME ": Internal Error: " fmt "\n" ,##args)

#define DEBUG(num, fmt, args...) \
  do { if (debug >= num) printk(KERN_DEBUG MOD_NAME ": " fmt "\n" ,##args); } while (0)

#define DEBUG_NN(num, fmt, args...) \
  do { if (debug >= num) printk(KERN_DEBUG MOD_NAME ": " fmt ,##args); } while (0)

#define DEBUG_CT(num, fmt, args...) \
  do { if (debug >= num) printk(fmt ,##args); } while (0)

struct cx25840_state {
  int norm;
  int input;
  int enable;
  int bright;
  int contrast;
  int hue;
  int sat;
  int audio;
  
  /** Stores the desired state of each register. Dirty registers may still need to be updated. */
  u8 reg_cache[number_of_registers];
  
  /** Stores a dirty bit for each register. */
  u8 reg_dirty[(number_of_registers+7)/8];
  
  // I2C Write wrapper buffer, used for debugging...
  u8 mem[4*1024];
  u8 dirt[4*1024];
};

/** Macro to set a register's dirty flag. */
#define MARK_REG_DIRTY(state,r) ((state)->reg_dirty[(r)/8]|= (1<<((r)&7)))
/** Macro to clear a register's dirty flag. */
#define MARK_REG_CLEAN(state,r) ((state)->reg_dirty[(r)/8]&= ~(1<<((r)&7)))
/** Macro to test a register's dirty flag. */
#define IS_REG_DIRTY(state,r) ((state)->reg_dirty[(r)/8]&(1<<((r)&7)))



#define CX25840_SVIDEO 0
#define CX25840_COMPOSITE 1
#define CX25840_TUNER 2
#define CX25840_FM 3
#define CX25840_AUDIOIN 4

static int cx25840_input_layout[] = {-1,-1,-1,-1,-1,-1,-1};

int pvr_150_input_layout[] = {CX25840_SVIDEO,
			CX25840_SVIDEO,
			CX25840_COMPOSITE,
			CX25840_COMPOSITE,
			CX25840_COMPOSITE,
			CX25840_COMPOSITE,
			CX25840_TUNER,
			-1};

int pvr_500_input_layout[] = {CX25840_SVIDEO,
			CX25840_SVIDEO,
			CX25840_COMPOSITE,
			CX25840_COMPOSITE,
			CX25840_COMPOSITE,
			CX25840_COMPOSITE,
			CX25840_TUNER,
			CX25840_TUNER};

int pg_600_input_layout[] = {CX25840_SVIDEO,
			CX25840_TUNER,
			CX25840_COMPOSITE,
			CX25840_FM,
			CX25840_AUDIOIN,
			-1,-1};

/* ----------------------------------------------------------------------- */
void cx25840_analyze(int ptr, __u8 byte) {
  int  i;
  __u8 covered=0;
  for(i=0; i<number_of_settings; i++) {
    int sbit=8*ptr-8*CX25840_REGISTER_ADDRESS(CX25840_SETTING_REGISTER_INDEX(i))-CX25840_SETTING_START_BIT(i);
    int lbit=CX25840_SETTING_BIT_LENGTH(i);
    if (sbit<lbit && sbit+8>0) {
      covered|=(0xffffffff>>(32-lbit))<<-sbit;
      if (CX25840_SETTING_TYPE(i)!=RO && ptr!=0x0802) {
	unsigned int val=((sbit>0)?(byte<<sbit):(byte>>-sbit))&(0xffffffff>>(32-lbit));
	value_map *ap=CX25840_SETTING_MAPPING(i);
	while(ap && ap->name && ap->value!=val) ap++;
	//      if (val!=CX25840_SETTING_DEFAULT_VALUE(i))
	DEBUG_NN(3, "     CX25840_SET_%s(0x%04x)", CX25840_SETTING_NAME(i), val);
	{ int ii=40-strlen(CX25840_SETTING_NAME(i)); while (ii-->0) DEBUG_CT(3, " "); }
	DEBUG_CT(3, " // '%s' (Default 0x%04x) Reg: 0x%04x Start bit: %d Bit length: %d\n",
	       (ap && ap->name)?ap->name:"*",
	       CX25840_SETTING_DEFAULT_VALUE(i),
	       CX25840_REGISTER_ADDRESS(CX25840_SETTING_REGISTER_INDEX(i)),
	       CX25840_SETTING_START_BIT(i),
	       CX25840_SETTING_BIT_LENGTH(i));
      }
    }
  }
}

static int analyze=0;

/**
 * NOTE: This function must be placed before the following #define that 
 * redefines the i2c_master_send function.
 */
static inline int i2c_master_send_wrap(struct i2c_client *client, u8 *b, int len) {
  struct cx25840_state *s=i2c_get_clientdata(client);
  int i;
  int ptr=b[0]*256+b[1];
  if (client->addr==0x44) {
    for(i=2;i<len;i++) {
      if (ptr<4*1024) {
	s->mem[ptr]=b[i];
	s->dirt[ptr]=1;
	if(ptr!=0x802) {
	  if (analyze) cx25840_analyze(ptr,b[i]);
	}
      } else {
	IE("I2C write to non existing register 0x%04x=0x%02x",ptr,b[i]);
      }
      if (~s->mem[0]&0x20) ptr++;
    }	
  } else {
    DEBUG(0, "*** I2C Write to no cx25840 device 0x%02x.", client->addr);
  }
  return i2c_master_send(client, b, len);
}


/**
 * Special call to send a direct I2C command (usually to a device other than the cx25840),
 * for testing and development.
 * 
 * NOTE: This function must be placed before the following #define that 
 * redefines the i2c_master_send function.
 */
static inline int execute_wi2c( struct i2c_client *client, int addr, u8 *b, int len )
{
	int save_addr= client->addr;
	int retval;
	client->addr= addr;
	retval= i2c_master_send( client, b, len );
	client->addr= save_addr;
	return retval;
}


#if 0
static void dump_mem(struct i2c_client *client) {
  struct cx25840_state *s=i2c_get_clientdata(client);
  int ptr;
  int start;
  char str[5*16+50];
  char *trg;
  DEBUG(3, "Dump of all modified regs");
  for(ptr=0;ptr<4*1024;ptr++) {
    if (s->dirt[ptr]) {
      int c=1;
      while((ptr+c)%16 && s->dirt[ptr+c]) c++;
      trg=str;
      start=ptr;
      trg+=sprintf(trg, " WI2C(%2d, cx25840,  0x%02X,0x%02X  ", c, ptr/256, ptr%256);
      while (c) { trg+=sprintf(trg, ",0x%02x", s->mem[ptr]); ptr++ ; c-- ; } ptr--;
      trg+=sprintf(trg, "),");
      DEBUG(3, "%s", str);
    }
  }	
} 
#endif

#define i2c_master_send i2c_master_send_wrap

#define I_cx25840           0x44
#define wm8775              0x1b

/**
 *  Update a register with the given value. The update is not effected until execute_setting is called.
 *  @param client Client to update for.
 *  @param setting Index into the setting enum.
 *  @param value New value for this setting.
 */
void update_setting( struct i2c_client *client, __u16 setting, __u32 value )
{
	struct cx25840_state *state=i2c_get_clientdata(client);
	__u32  debvalue=value;
   
   // map setting to a register
	int i= CX25840_SETTING_REGISTER_INDEX(setting);
	int s= CX25840_SETTING_START_BIT(setting);
	int l= CX25840_SETTING_BIT_LENGTH(setting);

   // write value into register shadow buffer marking dirty register bytes   
	int modified=0;
	while( l>0 ) {
		__u8 old;
		__u8 newb;
		__u8 mask= (0xff>>(8-(l>8?8:l)))<<s;
      
		MARK_REG_DIRTY( state, i );
      
		old=state->reg_cache[i];
		newb= (old&~mask)|(mask&(value<<s));
		modified|= (old!=newb);
		state->reg_cache[i]= newb;
      
		l-=8-s;
		value>>=(8-s);
		s=0;
		i++;
	}

	DEBUG(3, "%s %s Updating %s (%d) to 0x%04x, ri %d, sb %d, bl %d", CX25840_SETTING_TYPE(setting)==RW?" rw ":"!RW!", modified?"!MOD!":" mod ", 
	      CX25840_SETTING_NAME(setting), setting, debvalue,CX25840_SETTING_REGISTER_INDEX(setting),CX25840_SETTING_START_BIT(setting),CX25840_SETTING_BIT_LENGTH(setting));

}

/**
 * Execute the settings queued for the given client.
 */
int execute_settings( struct i2c_client *client )
{
	struct cx25840_state *state=i2c_get_clientdata(client);
	__u8 buf[ 18 ];   // buffer up to 16 bytes at a time
	int i;
   
	for( i= 0; i<number_of_registers; ) {
		if( IS_REG_DIRTY( state, i ) ) {
			int ret;
			int l;
         
         // prepare address of the first dirty register
			buf[0]= (CX25840_REGISTER_ADDRESS(i)>>8)&0xff;
			buf[1]= CX25840_REGISTER_ADDRESS(i)&0xff;
         
         // find number of bytes to send in one command (max 16)
			l= 0;
			while( l+2<sizeof( buf )
						    && (i+l)<number_of_registers
						    && IS_REG_DIRTY( state, i+l )
						    && (CX25840_REGISTER_ADDRESS(i)+l==CX25840_REGISTER_ADDRESS(i+l)) )
			{
				buf[ l+2 ]= state->reg_cache[ i+l ];
				l++;
			}
         
			{
				char buff[ sizeof( buf )*5+1];
				char *p=buff;
				int ii;
				for(ii= 0; ii<l; ii++) p+= sprintf(p,"0x%02x,", buf[ii+2] );
				DEBUG(3, "Sending to 0x%04x len %d %s", CX25840_REGISTER_ADDRESS(i), l, buff);
			}  
         
			ret= i2c_master_send(client, buf, l+2 );
         
			if( ret!=l+2 ) { 
				ERR("I2C sequence write failed, length %d, ret %d. Stopping sequence!", l+2, ret); 
				return -EINVAL;
			}
         
			i+= l;
		}
		else
		{
			i++;
		}
	}
	
	// clear dirty bits
	memset( state->reg_dirty, 0, sizeof( state->reg_dirty ) );
	return 0; // happy!
}



/**
 * Load the registers with default values and send them.
 */
int set_default_values(struct i2c_client *client) {
	struct cx25840_state *state=i2c_get_clientdata(client);
	int i;
   
	memset( state->reg_dirty, 0, sizeof( state->reg_dirty ) );
   
	for( i= 0; i<number_of_registers; i++ )
	{
		state->reg_cache[i]=CX25840_REGISTER_DEFAULT( i );
		MARK_REG_DIRTY( state, i );
	}
   
	return execute_settings( client );
}
   


__u32 read_setting(struct i2c_client *client, __u16 setting) {
  struct cx25840_state *state=i2c_get_clientdata(client);
  int i;
  int s;
  int l;
  int rl;
  __u32 value;
  int ret;
  if (setting>=number_of_settings) {
    IE("Ilegal setting number 0x0%4x", setting);
  }
  i=CX25840_SETTING_REGISTER_INDEX(setting);
  s=CX25840_SETTING_START_BIT(setting);
  l=CX25840_SETTING_BIT_LENGTH(setting);
  rl=(s+l+7)/8;
  if (CX25840_SETTING_TYPE(setting)!=RW) {
    char buff[2]; buff[0]=CX25840_REGISTER_ADDRESS(i)>>8; buff[1]=CX25840_REGISTER_ADDRESS(i)&0xff;
    ret=i2c_master_send(client, buff, 2);
    if(ret!=2) {
      ERR("I2C write failed, length %d, ret %d", l+2, ret); 
    } else {
      ret=i2c_master_recv(client, &state->reg_cache[i], rl);
      if(ret!=rl) { ERR("I2C sequence recv failed, length %d, ret %d", l+2, ret); } 
    }
  }
  value=state->reg_cache[i]>>s;
  do {
    s-=8; i++; rl--;   
    if(rl==0) break;
    value|=(state->reg_cache[i]<<-s);
  } while (1);
  return value&(0xffffffff>>(32-l));
}

static void do_cx25840_dump(struct i2c_client *client)  {
  int addr;
  u8 cmd[2], byte;
  char buff[5*256+10], *sptr=0, *lptr=0;
  int l=0;
  
  addr=0;
  DEBUG(3, "Dump of regs != 0x00");
  while (addr<4*1024) {
    cmd[0]=addr/256;
    cmd[1]=addr%256;
    i2c_master_send(client, cmd, 2);
    i2c_master_recv(client, &byte, 1);
    if (byte) { 
      if (l==0) {
	sptr=buff+sprintf(buff, "D(0x%04x, ", addr);
	lptr=sptr; sptr+=5;
      }
      sptr+=sprintf(sptr, ",0x%02x", byte);
      l++;
    }
    addr++;
    if (l&&(!byte||!addr%16||addr==4*1024)) {
      sprintf(lptr, "%4d", l); lptr[4]=' ';
      DEBUG(3, "%s);", buff);
      l=0;
    }
  }
}

static void u8str2hexstr(unsigned char *buf, unsigned int len, char *dst) {
  while (len--) dst+=sprintf(dst, "%02x", *buf++);
}


static int start_fw_load( struct i2c_client *client ) {
	CX25840_TAG('F','W',' ','D','L',' ','S','t','a','r','t')
	CX25840_SET_DL_ADDR_LB(0x0000)                               // '*' (Default 0x0000) Reg: 0x0800 Start bit: 0 Bit length: 8
	CX25840_SET_DL_ADDR_HB(0x0000)                               // '*' (Default 0x0000) Reg: 0x0801 Start bit: 0 Bit length: 8
	CX25840_SET_DL_MAP(0x0003)                                   // '*' (Default 0x0000) Reg: 0x0803 Start bit: 0 Bit length: 2
	CX25840_SET_DL_AUTO_INC(0x0000)                              // 'Auto increment address on write' (Default 0x0000) Reg: 0x0803 Start bit: 2 Bit length: 1
	CX25840_SET_DL_ENABLE(0x0001)                                // 'Enable' (Default 0x0000) Reg: 0x0803 Start bit: 3 Bit length: 1
	CX25840_SET_START_MICROCNTL(0x0000)                          // '*' (Default 0x0000) Reg: 0x0803 Start bit: 4 Bit length: 1
	CX25840_SET_AUTO_INC_DIS(0x0001)                             // 'Do not increment the address' (Default 0x0000) Reg: 0x0000 Start bit: 5 Bit length: 1
	return execute_settings( client );
}
  
static int end_fw_load( struct i2c_client *client ) {
	CX25840_TAG('F','W',' ','D','L',' ','S','t','o','p')
	CX25840_SET_AUTO_INC_DIS(0x0000)                             // 'Do the auto-address increment' (Default 0x0000) Reg: 0x0000 Start bit: 5 Bit length: 1
	CX25840_SET_DL_ENABLE(0x0000)                                // 'Disable' (Default 0x0000) Reg: 0x0803 Start bit: 3 Bit length: 1
	return execute_settings( client );
}



static int load_aud_fw(struct i2c_client *client)  {
  kernel_filep filep;
  long tsize, ret;
  loff_t file_offset=0;
  char *audio_fw_files[]=AUDIO_FW_FILES;
  int file_index;
  // incorporated David Gesswein's 4kstacks patch here...
  char *fw_buffer;
#define FW_BUFFER_SIZE (1024)
  int size;
  struct scatterlist sg[8];
  char result[64];
  char result_ascii[2*sizeof(result)+1];
  struct crypto_tfm *md5;
  struct crypto_tfm *sha1;

  mm_segment_t fs = get_fs();
  
  fw_buffer = kmalloc(FW_BUFFER_SIZE, GFP_KERNEL);
  if (fw_buffer == 0) {
	  ERR("load_aud_firmware out of memory");
	  return -ENOMEM;
  }
  fw_buffer[0] = 0x08;
  fw_buffer[1] = 0x02;

  set_fs(get_ds());

  md5 = crypto_alloc_tfm("md5", 0);  if (md5 == NULL) INFO("Failed to md5 digester, skipping it.");
  sha1 = crypto_alloc_tfm("sha1", 0);  if (sha1 == NULL) INFO("Failed to load sha1 digester, skipping it.");
    
  for(file_index=0;file_index<(fw_file_name?1:ARRAY_SIZE(audio_fw_files));file_index++) {
    char *file_name=fw_file_name?fw_file_name:audio_fw_files[file_index];
    filep = kernel_file_open(file_name, 0, 0);
    if(kernel_file_is_err(filep)) {
      INFO("Unable to open FW file '%s'.", file_name);
      continue;
    }

    if( start_fw_load( client )<0 )
    {
	    return -EINVAL;
    }
    
    if (md5) crypto_digest_init (md5);
    if (sha1) crypto_digest_init (sha1);
    sg[0].page = virt_to_page (&fw_buffer[2]);
    sg[0].offset = offset_in_page (&fw_buffer[2]);

    tsize=0;
  
    do {
      size=FW_BUFFER_SIZE-2;
      ret=kernel_file_read(filep, &fw_buffer[2], size,  &file_offset);
      if (ret<0) {
        ERR("Failed to read FW file '%s' after reading %ld bytes.", file_name, tsize);
        goto fail_next_file;
      }
      
      if (ret==0) goto file_loaded;

      tsize+=ret;
      size=ret+2;

      sg[0].length = ret;
      if (md5) crypto_digest_update (md5, sg, 1);
      if (sha1) crypto_digest_update (sha1, sg, 1);

      ret=i2c_master_send(client, fw_buffer, size);
      if (ret!=size) {
        ERR("Failed to upload FW chunk, size %d, uploaded %ld bytes.", size-2, tsize-size+2);
	goto fail_next_file;
      }

    } while (1);
    
  file_loaded:

    end_fw_load( client );
    kernel_file_close(filep);
    set_fs(fs);
    INFO("FW image '%s' of size %ld loaded.", file_name, tsize);

    if (md5) {
      memset (result, 0, 64);
      crypto_digest_final (md5, result);
      u8str2hexstr(result, crypto_tfm_alg_digestsize (md5), result_ascii);
      INFO("FW image md5 digest: %s", result_ascii);
      crypto_free_tfm (md5);
    }    
    if (sha1) {
      memset (result, 0, 64);
      crypto_digest_final (sha1, result);
      u8str2hexstr(result, crypto_tfm_alg_digestsize (sha1), result_ascii);
      INFO("FW image sha1 digest: %s", result_ascii);
      crypto_free_tfm (sha1);
    }
    
    kfree( fw_buffer );
    return 0;    
    
  fail_next_file:
    kernel_file_close(filep);
  }

  end_fw_load( client );

  ERR("Failed to load any FW image file");
  
  kfree( fw_buffer );
  set_fs(fs);
  return -EINVAL;
}

static int cx25840_initialize(struct i2c_client *client) {
  
  // CX25840_SET_AUTO_INC_DIS(0),    // Do the auto-address increment
  // CX25840_EXECUTE,
  
  /*
    static void cx25840_reset(struct i2c_client *client) {
    
    CX25840_TAG('R','e','s','e','t'),
    WI2C(3, I_cx25840, 0x00,0x00,   0x01),       
    WI2C(3, I_cx25840, 0x00,0x00,   0x00),
    
    WI2C(3, I_cx25840, 0x00,0x5A,   0x87),
    WI2C(3, I_cx25840, 0x00,0x5B,   0x06),
    WI2C(3, I_cx25840, 0x00,0x59,   0x21),
    CX25840_TAG('D','e','l','a','y'),      // Ref says to sleep 10us, which is nothing in i2c time, hence not needed...
    WI2C(3, I_cx25840, 0x00,0x59,   0xE1),
    WI2C(3, I_cx25840, 0x00,0x59,   0x20),
    WI2C(3, I_cx25840, 0x00,0x59,   0x21),
    WI2C(3, I_cx25840, 0x00,0x5B,   0x10),
    END
  */
  
    if( set_default_values(client)<0 )
    {
	    return -EINVAL;
    }	    
		
  
    CX25840_TAG('B','o','o','t','.')
    CX25840_SET_VD_SOFT_RST(1)     // Video assert
    CX25840_SET_SOFT_RESET(1)      // Soft reset assert
    CX25840_SET_START_MICROCNTL(0) // Brodcast audio microcontroller is running.
    CX25840_EXECUTE

    CX25840_SET_CH_SEL_ADC2(0x0001)                              // 'Analog input CH{3}' (Default 0x0000) Reg: 0x0102 Start bit: 1 Bit length: 1
    CX25840_SET_CH_1__SOURCE(0x0006)                             // '*' (Default 0x0000) Reg: 0x0103 Start bit: 0 Bit length: 3
    CX25840_SET_CH_3__SOURCE(0x0001)                             // 'CVBS8/C4/Pr2' (Default 0x0000) Reg: 0x0103 Start bit: 6 Bit length: 2
    CX25840_SET_AUX_PLL_POST(0x0020)                             // '*' (Default 0x0010) Reg: 0x010b Start bit: 0 Bit length: 6
    CX25840_SET_AUX_PLL_FRAC1(0x00e5)                            // '*' (Default 0x0009) Reg: 0x0110 Start bit: 0 Bit length: 8
    CX25840_SET_AUX_PLL_FRAC2(0x00d6)                            // '*' (Default 0x00d7) Reg: 0x0111 Start bit: 0 Bit length: 8
    CX25840_SET_VID_OUT_EN(0x0001)                               // 'Enable' (Default 0x0000) Reg: 0x0115 Start bit: 2 Bit length: 1
    CX25840_SET_PIXCLK_OUT_EN(0x0001)                            // 'Enable' (Default 0x0000) Reg: 0x0115 Start bit: 3 Bit length: 1
    CX25840_SET_SA_SDOUT_OUT_EN(0x0001)                          // 'Enable' (Default 0x0000) Reg: 0x0115 Start bit: 7 Bit length: 1
    CX25840_SET_SA_BCLKOUT_OUT_EN(0x0001)                        // 'Enable' (Default 0x0000) Reg: 0x0116 Start bit: 0 Bit length: 1
    CX25840_SET_SA_WCLKOUT_OUT_EN(0x0001)                        // 'Enable' (Default 0x0000) Reg: 0x0116 Start bit: 1 Bit length: 1
    CX25840_SET_PLL_CLK_OUT_EN(0x0001)                           // 'Enable' (Default 0x0000) Reg: 0x0116 Start bit: 2 Bit length: 1
    CX25840_SET_VID_OUT_SPD(0x0002)                              // '*' (Default 0x0000) Reg: 0x0118 Start bit: 0 Bit length: 2
    CX25840_SET_GPI0_IN_SEL(0x0007)                              // '*' (Default 0x0049) Reg: 0x0124 Start bit: 0 Bit length: 4
    CX25840_SET_GPI2_IN_SEL(0x0006)                              // '*' (Default 0x0048) Reg: 0x0125 Start bit: 0 Bit length: 4
    CX25840_SET_GPI3_IN_SEL(0x000b)                              // '*' (Default 0x03f3) Reg: 0x0125 Start bit: 4 Bit length: 4
    CX25840_SET_SA_MCLK_DIV(0x0010)                              // '*' (Default 0x0000) Reg: 0x0127 Start bit: 0 Bit length: 6
    CX25840_SET_SA_MCLK_SEL(0x0001)                              // 'PLL_CLK/PRGM7 is divided by the alternate post-divider' (Default 0x0000) Reg: 0x0127 Start bit: 6 Bit length: 1
    CX25840_SET_VREG_D_1_0_(0x0000)                              // '1.20 V' (Default 0x0001) Reg: 0x0134 Start bit: 1 Bit length: 2
    CX25840_SET_S2DIFF_BIAS_1_0_(0x0002)                         // '*' (Default 0x0001) Reg: 0x0136 Start bit: 0 Bit length: 2
    CX25840_SET_FILTER_BIAS_1_0_(0x0002)                         // '*' (Default 0x0001) Reg: 0x0136 Start bit: 2 Bit length: 2
    CX25840_SET_DLL1_COMP_LT(0x0003)                                  // '*' (Default 0x0009) Reg: 0x015a Start bit: 1 Bit length: 3
    CX25840_SET_DLL1_CURRSET(0x0008)                                  // '*' (Default 0x0000) Reg: 0x015b Start bit: 1 Bit length: 4
    CX25840_SET_DLL2_COMP_LT(0x0003)                            // '*' (Default 0x0009) Reg: 0x015e Start bit: 1 Bit length: 3
    CX25840_SET_DLL2_CURRSET(0x0003)                            // '*' (Default 0x0000) Reg: 0x015f Start bit: 1 Bit length: 4
    CX25840_SET_ACFG_DIS(0x0000)                           // 'Disable' (Default 0x0000) Reg: 0x0400 Start bit: 5 Bit length: 1
    CX25840_SET_AUTO_SC_LOCK(0x0001)                             // 'Manual mode. Lock speed is determined by' (Default 0x0001) Reg: 0x0401 Start bit: 4 Bit length: 1
    CX25840_SET_COMB_NOTCH_MODE(0x0001)                          // 'Notch data is interpreted as chroma' (Default 0x000a) Reg: 0x0402 Start bit: 2 Bit length: 2
    CX25840_SET_OUT_MODE(0x0002)                                 // '*' (Default 0x0001) Reg: 0x0404 Start bit: 0 Bit length: 2
    CX25840_SET_MODE10B(0x0001)                                  // 'Luma and Chroma Output have 10 bits of resolution' (Default 0x0000) Reg: 0x0404 Start bit: 2 Bit length: 1
    CX25840_SET_VBIHACTRAW_EN(0x0001)                            // 'Enable' (Default 0x0000) Reg: 0x0404 Start bit: 3 Bit length: 1
    CX25840_SET_VIPCLAMP_EN(0x0001)                              // 'Enable' (Default 0x0000) Reg: 0x0406 Start bit: 4 Bit length: 1
    CX25840_SET_BRIGHT(0x00f4)                                   // '*' (Default 0x0000) Reg: 0x0414 Start bit: 0 Bit length: 8
    CX25840_SET_CNTRST(0x0079)                                   // '*' (Default 0x0080) Reg: 0x0415 Start bit: 0 Bit length: 8
    CX25840_SET_VS_INTRLACE(0x0001)                              // 'Interlace VS' (Default 0x0000) Reg: 0x041e Start bit: 3 Bit length: 1
    CX25840_SET_USAT(0x0071)                                     // '*' (Default 0x0080) Reg: 0x0420 Start bit: 0 Bit length: 8
    CX25840_SET_VSAT(0x0071)                                     // '*' (Default 0x0080) Reg: 0x0421 Start bit: 0 Bit length: 8
    CX25840_SET_HBLANK_CNT(0x2d84)                           // '*' (Default 0x007a) Reg: 0x0470 Start bit: 0 Bit length: 8
    CX25840_SET_BGDEL_CNT(0x005d)                                // '*' (Default 0x005b) Reg: 0x0473 Start bit: 0 Bit length: 8
    CX25840_SET_VBLANK_CNT(0x0024)                           // '*' (Default 0x0014) Reg: 0x0474 Start bit: 0 Bit length: 8
    CX25840_SET_VACTIVE_CNT(0x2404)                          // '*' (Default 0x0049) Reg: 0x0475 Start bit: 4 Bit length: 4
    CX25840_SET_V656BLANK_CNT(0x0028)                            // '*' (Default 0x0020) Reg: 0x0477 Start bit: 0 Bit length: 8
    CX25840_SET_LUMA_LPF_SEL(0x0002)                             // '*' (Default 0x0001) Reg: 0x047a Start bit: 6 Bit length: 2
    CX25840_SET_LCOMB_2LN_EN(0x0000)                             // 'Disable' (Default 0x0001) Reg: 0x047b Start bit: 1 Bit length: 1
    CX25840_SET_LCOMB_3LN_EN(0x0000)                             // 'Disable' (Default 0x0001) Reg: 0x047b Start bit: 2 Bit length: 1
    CX25840_SET_CCOMB_3LN_EN(0x0000)                             // 'Disable' (Default 0x0001) Reg: 0x047b Start bit: 6 Bit length: 1
    CX25840_SET_SC_STEP(0x0a8263)                              // '*' (Default 0x001f) Reg: 0x047c Start bit: 0 Bit length: 8
    CX25840_SET_VBI_OFFSET(0x0001)                               // '*' (Default 0x0000) Reg: 0x047f Start bit: 0 Bit length: 5
    CX25840_SET_COMB_PHASE_LIMIT(0x0014)                         // '*' (Default 0x0020) Reg: 0x049f Start bit: 0 Bit length: 8
    CX25840_SET_AUD_MODE_AUD_SYSTEM(0x000f)                      // '*' (Default 0x0000) Reg: 0x0808 Start bit: 0 Bit length: 4
    CX25840_SET_AUD_STANDARD(0x000f)                             // '*' (Default 0x0000) Reg: 0x0808 Start bit: 4 Bit length: 4
    CX25840_SET_PREF_MODE(0x0004)                                // '*' (Default 0x0000) Reg: 0x0809 Start bit: 0 Bit length: 4
    CX25840_SET_PATH1_AVC_RMS_CON(0x0007)                        // '*' (Default 0x0049) Reg: 0x08d0 Start bit: 4 Bit length: 4
    CX25840_SET_PATH1_AVC_AT(0x0003)                             // '*' (Default 0x0009) Reg: 0x08d1 Start bit: 4 Bit length: 4
    CX25840_SET_PATH1_AVC_RT(0x0006)                             // '*' (Default 0x0048) Reg: 0x08d2 Start bit: 0 Bit length: 4
    CX25840_SET_SRC1_MUTE_EN(0x0001)                             // 'Enable' (Default 0x0000) Reg: 0x08d3 Start bit: 1 Bit length: 1
    CX25840_SET_SA_MUTE_EN(0x0001)                               // 'Enable' (Default 0x0000) Reg: 0x08d3 Start bit: 2 Bit length: 1
    CX25840_SET_PAR_MUTE_EN(0x0001)                              // 'Enable' (Default 0x0000) Reg: 0x08d3 Start bit: 3 Bit length: 1
    CX25840_SET_AC97_MUTE_EN(0x0001)                             // 'Enable' (Default 0x0000) Reg: 0x08d3 Start bit: 4 Bit length: 1
    CX25840_SET_SA_IN_WS_SEL(0x0001)                             // 'Left sample on SA_WCLK = 1' (Default 0x0000) Reg: 0x0914 Start bit: 5 Bit length: 1
    CX25840_SET_SA_IN_SONY_MODE(0x0001)                          // 'Sony mode: 1st SA_BCLK_IN rising edge after' (Default 0x0000) Reg: 0x0914 Start bit: 7 Bit length: 1
    CX25840_SET_SA_OUT_WS_SEL(0x0001)                            // 'Left sample on SA_WCLK = 1' (Default 0x0000) Reg: 0x0918 Start bit: 5 Bit length: 1
    CX25840_SET_SA_OUT_SONY_MODE(0x0001)                         // 'Sony mode: 1st SA_BCLK_OUT rising edge after' (Default 0x0000) Reg: 0x0918 Start bit: 7 Bit length: 1
    CX25840_SET_SA_OUT_MASTER_MODE(0x0001)                       // 'Master: SA_BCLK_OUT, and SA_WCLK_OUT are' (Default 0x0000) Reg: 0x0919 Start bit: 0 Bit length: 1


     CX25840_SET_VGA_SEL_CH2(0x0001)                              // 'Audio decoder drives VGA gain setting' (Default 0x0000) Reg: 0x0104 Start bit: 7 Bit length: 1
     CX25840_SET_VGA_SEL_CH3(0x0001)                              // 'Audio decoder drives VGA gain setting' (Default 0x0000) Reg: 0x0105 Start bit: 0 Bit length: 1
     CX25840_SET_AUD_LOCK_FREQ_SHIFT(0x0002)                      // '*' (Default 0x0000) Reg: 0x012e Start bit: 4 Bit length: 2
     CX25840_SET_ROE(0x0001)                                      // 'Receive FIFO overrun interrupt enabled' (Default 0x0000) Reg: 0x0214 Start bit: 1 Bit length: 1
     CX25840_SET_RSE(0x0001)                                      // 'Receive FIFO interrupt enabled' (Default 0x0000) Reg: 0x0214 Start bit: 4 Bit length: 1
     CX25840_SET_TSE(0x0001)                                      // 'Transmit FIFO/Idle interrupt enabled' (Default 0x0000) Reg: 0x0214 Start bit: 5 Bit length: 1
     CX25840_SET_IDID1_9_2__LOW(0x0080)                           // '*' (Default 0x0000) Reg: 0x0409 Start bit: 0 Bit length: 8
     CX25840_SET_TTX_PKTADRU_LN(0x000f)                           // '*' (Default 0x0457) Reg: 0x043e Start bit: 4 Bit length: 4
     CX25840_SET_VBI2_SDID(0x000a)                                // '*' (Default 0x03f2) Reg: 0x0440 Start bit: 4 Bit length: 4
     CX25840_SET_VBI3_SDID(0x000b)                                // '*' (Default 0x03f3) Reg: 0x0441 Start bit: 0 Bit length: 4
     CX25840_SET_VBI1_FIFO_MODE(0x0002)                           // '*' (Default 0x0008) Reg: 0x0457 Start bit: 4 Bit length: 3
     CX25840_SET_VBI1_CRIWIN(0x0054)                              // '*' (Default 0x002c) Reg: 0x045b Start bit: 0 Bit length: 7
     CX25840_SET_VBI2_FIFO_MODE(0x0003)                           // '*' (Default 0x0009) Reg: 0x0463 Start bit: 4 Bit length: 3
     CX25840_SET_HACTIVE_CNT(0x02d0)                              // '*' (Default 0x0000) Reg: 0x0471 Start bit: 4 Bit length: 10
     CX25840_SET_FIELD_COUNT(0x03c3)                          // '*' (Default 0x0000) Reg: 0x0480 Start bit: 0 Bit length: 10
     CX25840_SET_AGC_GAIN(0x049f)                                 // '*' (Default 0x0100) Reg: 0x0489 Start bit: 0 Bit length: 12
     CX25840_SET_SYNC_TIP_REDUCE(0x0001)                          // '*' (Default 0x0000) Reg: 0x04a1 Start bit: 1 Bit length: 6
     CX25840_SET_APL_DETECT_ENA(0x0001)                           // 'Forces the Y/C separation algorithm into notch mode' (Default 0x000a) Reg: 0x04b8 Start bit: 0 Bit length: 1
     CX25840_SET_DEMATRIX_SEL_CTL(0x0042)                         // '*' (Default 0x0000) Reg: 0x08ce Start bit: 0 Bit length: 8
     CX25840_SET_PATH1_EQ_BASS_VOL(0x0018)                        // '*' (Default 0x0000) Reg: 0x08d9 Start bit: 0 Bit length: 6
     CX25840_SET_PATH1_EQ_MID_VOL(0x0018)                         // '*' (Default 0x0000) Reg: 0x08da Start bit: 0 Bit length: 6
     CX25840_SET_PATH1_EQ_TREBLE_VOL(0x0018)                      // '*' (Default 0x0000) Reg: 0x08db Start bit: 0 Bit length: 6
     CX25840_SET_PATH1_SC_RMS_CON(0x0003)                         // '*' (Default 0x0009) Reg: 0x08dc Start bit: 0 Bit length: 4
     CX25840_SET_PATH1_SC_CR(0x0002)                              // '*' (Default 0x0008) Reg: 0x08dc Start bit: 4 Bit length: 3
     CX25840_SET_PATH1_SC_STEREO(0x0001)                          // 'Stereo Mode' (Default 0x0000) Reg: 0x08dc Start bit: 7 Bit length: 1
     CX25840_SET_PATH1_SC_AT(0x0003)                              // '*' (Default 0x0009) Reg: 0x08dd Start bit: 0 Bit length: 4
     CX25840_SET_PATH1_SC_RT(0x0003)                              // '*' (Default 0x0009) Reg: 0x08dd Start bit: 4 Bit length: 4
     CX25840_SET_PATH2_AVC_RMS_CON(0x0007)                        // '*' (Default 0x0049) Reg: 0x08e0 Start bit: 4 Bit length: 4
     CX25840_SET_PATH2_AVC_AT(0x0003)                             // '*' (Default 0x0009) Reg: 0x08e1 Start bit: 4 Bit length: 4
     CX25840_SET_PATH2_AVC_RT(0x0006)                             // '*' (Default 0x0048) Reg: 0x08e2 Start bit: 0 Bit length: 4
     CX25840_SET_PATH2_AVC_THRESH(0x7fff)                         // '*' (Default 0x0400) Reg: 0x08e6 Start bit: 0 Bit length: 16
     CX25840_SET_PATH2_EQ_BASS_VOL(0x0018)                        // '*' (Default 0x0000) Reg: 0x08e9 Start bit: 0 Bit length: 6
     CX25840_SET_PATH2_EQ_MID_VOL(0x0018)                         // '*' (Default 0x0000) Reg: 0x08ea Start bit: 0 Bit length: 6
     CX25840_SET_PATH2_EQ_TREBLE_VOL(0x0018)                      // '*' (Default 0x0000) Reg: 0x08eb Start bit: 0 Bit length: 6
     CX25840_SET_PATH2_SC_RMS_CON(0x0003)                         // '*' (Default 0x0009) Reg: 0x08ec Start bit: 0 Bit length: 4
     CX25840_SET_PATH2_SC_CR(0x0002)                              // '*' (Default 0x0008) Reg: 0x08ec Start bit: 4 Bit length: 3
     CX25840_SET_PATH2_SC_STEREO(0x0001)                          // 'Stereo Mode' (Default 0x0000) Reg: 0x08ec Start bit: 7 Bit length: 1
     CX25840_SET_PATH2_SC_AT(0x0003)                              // '*' (Default 0x0009) Reg: 0x08ed Start bit: 0 Bit length: 4
     CX25840_SET_PATH2_SC_RT(0x0003)                              // '*' (Default 0x0009) Reg: 0x08ed Start bit: 4 Bit length: 4
     CX25840_SET_Sample_Rate_Converter_Status1_REG(0x000a)        // '*' (Default 0x0000) Reg: 0x08f1 Start bit: 0 Bit length: 8
     CX25840_SET_Sample_Rate_Converter_Status2_REG(0x0002)        // '*' (Default 0x0000) Reg: 0x08f2 Start bit: 0 Bit length: 8
    CX25840_EXECUTE
		    
    CX25840_SET_VD_SOFT_RST(0)     // Video deassert
    CX25840_SET_SOFT_RESET(0)      // Soft reset release
    CX25840_SET_START_MICROCNTL(1) // Brodcast audio microcontroller is running.
    CX25840_EXECUTE
  
  analyze=1;  

  load_aud_fw(client);
  
  return 0;
}

static void log_status (struct i2c_client *client) {
  char *p;
  DEBUG(1, "Status: 0x%02x (REV_ID, Revision ID.)", read_setting(client,REV_ID));
  switch (read_setting(client,MOD_DET_STATUS0)) {
  case 0x00: p="Mono"; break;
  case 0x01: p="Stereo"; break;
  case 0x02: p="Dual"; break;
  case 0x04: p="Tri"; break;
  case 0x10: p="SAP"; break;
  case 0xFE: p="Forced Mode"; break;
  default: p="Not defined";
  }
  DEBUG(1, "Status: '%s' (Detected audio mode)",p);
  switch (read_setting(client,MOD_DET_STATUS1)) {
  case 0x00: p="Not defined"; break;
  case 0x01: p="EIAJ"; break;
  case 0x02: p="A2-M"; break;
  case 0x03: p="A2-BG"; break;
  case 0x04: p="A2-DK1"; break;
  case 0x05: p="A2-DK2"; break;
  case 0x06: p="A2-DK3"; break;
  case 0x07: p="A1 (6.0 MHz FM Mono)"; break;
  case 0x08: p="AM-L"; break;
  case 0x09: p="NICAM-BG"; break;
  case 0x0A: p="NICAM-DK"; break;
  case 0x0B: p="NICAM-I"; break;
  case 0x0C: p="NICAM-L"; break;
  case 0x0D: p="BTSC/EIAJ/A2-M Mono (4.5 MHz FMMono)"; break;
  case 0x0E: p="IF FM Radio"; break;
  case 0x0F: p="BTSC"; break;
  case 0x10: p="High-Deviation FM"; break;
  case 0x11: p="Very High-Deviation FM"; break;
  case 0xFD: p="Unknown audio standard"; break;
  case 0xFE: p="Forced audio standard"; break;
  case 0xFF: p="No detected audio standard"; break;
  default: p="Not defined";
  }
  DEBUG(1, "Status: '%s' (Detected audio standard)", p);
  DEBUG(1, "Status: Video signal%spresent", read_setting(client,VIDEO_PRESENT)?" ":" not ");
  DEBUG(1, "Status: 0x%02x (MICROCNTL_VIDEO_FORMAT, Detected video format)", read_setting(client,MICROCNTL_VIDEO_FORMAT));
  DEBUG(1, "Status: %sReady Filter auto tuning status", read_setting(client,TUNING_READY)?"":"Not ");
  DEBUG(1, "Status: 0x%02x (TUNE_OUT, The tuning code selected by the auto-tune algorithm.)", read_setting(client,TUNE_OUT));
  DEBUG(1, "Status: 0x%02x (MV_CDAT, Macrovision Copy Control Bits as described in the MacroVision)", read_setting(client, MV_CDAT));
  DEBUG(1, "Status: 0x%02x (MV_PSP, Macrovision Pseudo Sync Pulses detected)", read_setting(client,MV_PSP));
  DEBUG(1, "Status: 0x%02x (MV_CS, Macrovision Color Striping Detected)", read_setting(client,MV_CS));
  DEBUG(1, "Status: 0x%02x (MV_T3CS, A 1 indicates the presence of type 3 of the color stripe process. A)", read_setting(client,MV_T3CS));
  DEBUG(1, "Status: 0x%02x (MV_TYPE2_PAIR, Macrovision Type 2 pair detected)", read_setting(client,MV_TYPE2_PAIR));
  DEBUG(1, "Status: 0x%02x (AFD_FMT_STAT, Currently detected Format)", read_setting(client,AFD_FMT_STAT));
  DEBUG(1, "Status: 0x%02x (FIELD, Field status (even/odd))", read_setting(client,FIELD));
  DEBUG(1, "Status: 0x%02x (SRC_FIFO_OFLOW, Sample Rate Converter FIFO Overflow)", read_setting(client,SRC_FIFO_OFLOW));
  DEBUG(1, "Status: 0x%02x (SRC_FIFO_UFLOW, Sample Rate Converter FIFO Underflow)", read_setting(client,SRC_FIFO_UFLOW));
  DEBUG(1, "Status: 0x%02x (VSYNC, Vertical sync)", read_setting(client,VSYNC));
  DEBUG(1, "Status: 0x%02x (HLOCK, Horizontal lock status)", read_setting(client,HLOCK));
  DEBUG(1, "Status: 0x%02x (SRC_LOCK, Sample Rate Converter lock Status)", read_setting(client,SRC_LOCK));
  DEBUG(1, "Status: 0x%02x (VLOCK, Vertical lock status)", read_setting(client,VLOCK));
  DEBUG(1, "Status: 0x%02x (CSC_LOCK, Color Subcarrier lock status)", read_setting(client,CSC_LOCK));
  DEBUG(1, "Status: 0x%02x (AGC_LOCK, VGA lock status)", read_setting(client,AGC_LOCK));
  DEBUG(1, "Status: 0x%02x (VPRES, Active-high video present. Indication of the presence of a)", read_setting(client,VPRES));
  DEBUG(1, "Status: 0x%02x (SPECIAL_PLAY_N, Active-low special play mode (fast forward, rewind, pulse, or slow)", read_setting(client,SPECIAL_PLAY_N));
}



/**
 * Read a byte from i2c.
 */
int read_reg_byte( struct i2c_client * client, int addr )
{
	int ret;
	char buff[2]; 
	buff[0]= addr>>8;
	buff[1]= addr&0xff;
	
	ret= i2c_master_send(client, buff, 2);
	if( ret!=2 ) 
	{
		ERR("I2C write failed, length %d, ret %d", 1, ret); 
		return -1;
	}
	
	ret= i2c_master_recv( client, buff, 1 );
	if( ret!=1 ) 
	{ 
		ERR("I2C sequence recv failed, length %d, ret %d", 1, ret);
		return -1;
	}
	
	return buff[0];
}


/**
 * Retrieve all settings into the IOCTL buffer. The buffer is filled with one line per setting and
 * each line in SETTING=VALUE format.
 * @param state Our state.
 * @param reg Reference to the user buffer.
 */
static void decoder_get_reg( struct i2c_client *client, struct cx25840_state * state, struct cx25840_reg * reg )
{
	if( reg->index<0 || reg->index>=number_of_settings )
	{
		// exhausted, return empty.
		reg->name[0]= '\0';
		reg->description[0]= '\0';
		reg->value= 0;
		reg->index= -1;
	}
	else
	{
		int i= reg->index;
		
		int ri= CX25840_SETTING_REGISTER_INDEX(i);
		int startbit= CX25840_SETTING_START_BIT(i);
		int bitlen= CX25840_SETTING_BIT_LENGTH(i);
		int value= 0;
		int bitpos= 0;
		int b;
		int addr= CX25840_REGISTER_ADDRESS( ri );
		
		while( bitlen>0 )
		{
			if( CX25840_SETTING_TYPE( i )==RW )
			{
				b= state->reg_cache[ ri ];
			}
			else
			{
				b= read_reg_byte( client, addr );
			}
			
			if( b<0 )
			{
				value= -1;
				break;
			}
			
			b= (b>>startbit) & ((1<<bitlen)-1);
			value|= b<<bitpos;
			bitpos+= 8-startbit;
			bitlen-= 8-startbit;
			startbit= 0;
			addr++;
			ri++;
		}
		
		strcpy( reg->name, CX25840_SETTING_NAME( i ) );
		reg->value= value;
		
		if( value<0 )
		{
			sprintf( reg->description, "i2c receive failed for 0x%x", addr );
		}
		else
		{
			// lookup value in map
			value_map *ap= CX25840_SETTING_MAPPING(i);
			reg->description[0]= '\0';
			while( ap!=NULL && ap->name!=NULL )
			{
				if( ap->value==value )
				{
					strcpy( reg->description, ap->name );
					break;
				}
				
				ap++;
			}
		}
		
	}
}
	
/**
 * Set settings encoded in the IOCTL buffer.
 * The buffer will be parsed for SETTING=VALUE constructs separated by whitespace.
 * @param state Our state.
 * @param reg Reference to the user buffer.
 */
static int decoder_set_reg( struct i2c_client *client, struct cx25840_state * state, struct cx25840_reg * reg )
{
	// if index is invalid, lookup name
	if( reg->index<0 || reg->index>=number_of_settings )
	{
		int i;
		for( i= 0; i<number_of_settings; i++ )
		{
			if( strcmp( reg->name, CX25840_SETTING_NAME( i ) )==0 )
			{
				reg->index= i;
				break;
			}
		}
		
		if( reg->index<0 || reg->index>=number_of_settings )
		{
			// still wrong
			return -EINVAL;
		}
	}
	
	// set the value
	update_setting( client, reg->index, reg->value );
	execute_settings( client );
	return 0;
}
	


/* ============ SAA7715 AUDIO settings (end) ============= */

static int cx25840_command (struct i2c_client *client, unsigned int cmd, void *arg) {
  struct cx25840_state *state = i2c_get_clientdata(client);
  
  log_status(client);
  switch (cmd) {  
  case 0: break;
  
	case DECODER_GET_REG_TEXT:
		DEBUG(1, "decoder get registers");
		decoder_get_reg( client, state, (struct cx25840_reg *)arg );
		break;
	
	case DECODER_SET_REG_TEXT: 
		DEBUG(1, "decoder set registers");
		// set reg returns result code
		return decoder_set_reg( client, state, (struct cx25840_reg *)arg );

  case DECODER_SET_SIZE: {
    // Used video_window because it has height/width and is already defined 
    struct video_window *wind = arg;
    int HSC, VSC;
    int Vsrc, Hsrc;
    int filter;
    
    Vsrc=read_setting(client, VACTIVE_CNT);
    Hsrc=read_setting(client, HACTIVE_CNT);

    if ( (wind->width*16<Hsrc) || (Hsrc < wind->width) ||
	 (wind->height*8<Vsrc) || (Vsrc < wind->height)) {
      ERR("Decoder set size: ignoring bad config request Hsrc=%d Vsrc=%d width=%d height=%d", Hsrc, Vsrc, wind->width, wind->height);
      return -EINVAL;
    }
    
    HSC = (Hsrc*(1<<20))/wind->width-(1<<20);
    VSC = (1<<16)-(Vsrc*(1<<9)/wind->height-(1<<9));
    
    if (wind->height >= 385) filter=0;
    else if (wind->height >= 193) filter=1;
    else filter=3;
    
    DEBUG(0, "Decoder set size %dx%d (from %dx%d) -> scale  %ux%u, filter %d", wind->width, wind->height, Hsrc, Vsrc, HSC, VSC, filter);

	CX25840_TAG('S','c','a','l','e')
	CX25840_SET_VS_INTRLACE(1)
	CX25840_SET_VFILT(filter)
	CX25840_SET_HSCALE(HSC)
	CX25840_SET_VSCALE(VSC)
	CX25840_EXECUTE
    break;
  }
  case DECODER_DUMP: {
    DEBUG(1, "decoder dump");
    do_cx25840_dump(client);
  }
  case DECODER_GET_CAPABILITIES: {
    struct video_decoder_capability *cap = arg;
    
    DEBUG(1, "decoder get capabilities");
    
    cap->flags = VIDEO_DECODER_PAL |
      VIDEO_DECODER_NTSC |
      VIDEO_DECODER_SECAM |
      VIDEO_DECODER_AUTO |
      VIDEO_DECODER_CCIR;
    cap->inputs = 8;
    cap->outputs = 1;
    break;
  }
    
  case DECODER_SET_AUDIO: {
    int *iarg = arg;
    u32 pinc;
    int path1;
    int ch_sel_adc2;
    int vidfmt= state->norm;	// = cx25840_get_vid_fmt( client );
    int audio_std = audiofmt;
    
    DEBUG(1, "set audio: 0x%02x", *iarg);
    
    // not sure if the following is correct, I just took it from the SET_INPUT methods
    switch( (state->input>=0 && state->input<8)? cx25840_input_layout[state->input]: -1 )
    {
    case CX25840_COMPOSITE: 
	pinc= 41942880; 
	path1= 2;
	ch_sel_adc2= 0;
	break;
	
    case CX25840_SVIDEO: 
	pinc= state->input==0? 57266012: 41942880;
	path1= 2;
	ch_sel_adc2= 0;
	break;
	
    case CX25840_TUNER:
	pinc= 41942880;		// should this be 41246400 for PAL?
	path1= (vidfmt==VIDEO_MODE_NTSC && cardtype!=2)? 2: 0;
	ch_sel_adc2= (vidfmt==VIDEO_MODE_NTSC)? 0: 1;
	if (audio_std==0xf && vidfmt==VIDEO_MODE_NTSC)
		audio_std = 1;
	break;
	
    default:
	    // no input set: do nothing, we'll come back to this after an input is set
	DEBUG(1, "invalid input setting 0x%02x", state->input );
	return -EINVAL;
    }    
  
    switch (*iarg) {
    case DECODER_AUDIO_32_KHZ:  pinc/=320; break;
    case DECODER_AUDIO_441_KHZ: pinc/=441; break;
    case DECODER_AUDIO_48_KHZ:  pinc/=480; break;
    default:
      DEBUG(1, "invalid audio setting 0x%02x", *iarg);
      return -EINVAL;
    }
    
    state->audio = *iarg; 
    
    CX25840_TAG('S','a','m','p','l','e',' ','r','a','t','e')
    CX25840_SET_CH_SEL_ADC2( ch_sel_adc2 )
    CX25840_SET_PATH1_SEL_CTL( path1 )
    CX25840_SET_SRC3_PHASE_INC( pinc ) 	  
    CX25840_SET_AUD_STANDARD( audio_std )
    CX25840_EXECUTE
    break;
  }
    
  case DECODER_GET_STATUS: {
    int *iarg = arg;
    int res;    
    res = 0;
    if (read_setting(client, HLOCK)) res |= DECODER_STATUS_GOOD;
    switch (state->norm) {
    case VIDEO_MODE_NTSC:  res |= DECODER_STATUS_NTSC; break;
    case VIDEO_MODE_PAL:   res |= DECODER_STATUS_PAL; break;
    case VIDEO_MODE_SECAM: res |= DECODER_STATUS_SECAM; break;
    case VIDEO_MODE_AUTO:
    default:
      switch (read_setting(client, AFD_FMT_STAT)) {
      case 0x0001: // NTSC-M
      case 0x0008: // NTSC-J
      case 0x0009: // NTSC-4.43
	res |= DECODER_STATUS_NTSC; break;
      case 0x0040: // PAL-BDGHI
      case 0x0041: // PAL-M
      case 0x0048: // PAL-N
      case 0x0049: // PAL-NC
      case 0x03e8: // PAL-60
	res |= DECODER_STATUS_PAL; break;
      case 0x044c: // SECAM
	res |= DECODER_STATUS_SECAM; break;
      }
      break;
    }
    // Macrovision check??? if (0x40C != 0) { res |= DECODER_STATUS_COLOR; }
    *iarg = res;
    break;
  }
    
  case DECODER_SET_NORM: {
    int *iarg = arg;
    switch (*iarg) {
    case VIDEO_MODE_NTSC:
      DEBUG(1, "decoder set norm NTSC");
      break;
    case VIDEO_MODE_PAL:
      DEBUG(1, "decoder set norm PAL");
      break;
    case VIDEO_MODE_SECAM:
      DEBUG(1, "decoder set norm SECAM");
      break;
    default:
      DEBUG(1, "Unknown video mode!!!");
      return -EINVAL;
    }
    state->norm = *iarg;
    /* switch audio mode too! */
    cx25840_command(client, DECODER_SET_AUDIO, &state->audio);
    break;
  }
    
  case DECODER_SET_INPUT: {
    int *iarg = arg;
    DEBUG(1, "decoder set input (%d)", *iarg);
    /* inputs from 0-9 are available*/
    if (*iarg < 0 || *iarg > 7) { 
      return -EINVAL;
    }
    
    if (state->input != *iarg) {
      state->input = *iarg;
      switch (cx25840_input_layout[state->input]) {
      case CX25840_COMPOSITE: {
	  CX25840_TAG('C','o','m','p','o','s','i','t','e')
	  CX25840_SET_START_MICROCNTL(0x0000)
	  CX25840_SET_SOFT_RESET(0x0001)
	  CX25840_SET_VD_SOFT_RST(0x0001)
	  CX25840_EXECUTE
	  
	  CX25840_SET_DUAL_MODE_ADC2(0x0000)
	  CX25840_SET_CHIP_ACFG_DIS(0x0000)
	  CX25840_SET_CH_1__SOURCE(0x0002)
	  CX25840_SET_CH_2__SOURCE(0x0000)
	  CX25840_SET_CH_3__SOURCE(0x0000)
	  CX25840_SET_AUX_PLL_POST(0x0018)
	  CX25840_SET_PATH1_AVC_RMS_CON(0x0001)
	  CX25840_SET_PATH1_AVC_CR(0x0000)
	  CX25840_SET_PATH1_AVC_STEREO(0x0000)
	  CX25840_SET_PATH1_AVC_AT(0x0001)
	  CX25840_SET_PATH1_AVC_RT(0x0001)
	  CX25840_SET_PATH1_AVC_CG(0x0000)
	  CX25840_SET_SOFT1_MUTE_EN(0x0001)
	  CX25840_SET_SRC1_MUTE_EN(0x0000)
	  CX25840_SET_SA_MUTE_EN(0x0000)
	  CX25840_SET_PAR_MUTE_EN(0x0000)
	  CX25840_SET_AC97_MUTE_EN(0x0000)
	  CX25840_EXECUTE
	  
	  CX25840_SET_START_MICROCNTL(0x0000)
	  CX25840_SET_SOFT_RESET(0x0000)
	  CX25840_SET_VD_SOFT_RST(0x0000)
	  
	  CX25840_EXECUTE
	break;
      }
      case CX25840_SVIDEO: {
	  CX25840_TAG('S','-','v','i','d','e','o')
	  CX25840_SET_START_MICROCNTL(0x0000)
	  CX25840_SET_SOFT_RESET(0x0001)
	  CX25840_SET_VD_SOFT_RST(0x0001)
	  CX25840_EXECUTE
	  
	  CX25840_SET_DUAL_MODE_ADC2(0x0000)            // 'Normal mode' (Default 0x00, Register 0x102.2-3)
	  CX25840_SET_CHIP_ACFG_DIS(0x0000)             // 'Allow VID_PLL_INT, VID_PLL_FRAC, and AFE control' (Default 0x00, Register 0x102.4-5)
	  CX25840_SET_CH_1__SOURCE(0x0000)              // 'CVBS1/Y1' (Default 0x00, Register 0x103.0-3)
	  CX25840_SET_CH_2__SOURCE(0x0001)              // 'CVBS5/C1/Pb2' (Default 0x00, Register 0x103.4-6)
	  CX25840_SET_CH_3__SOURCE(0x0000)              // 'CVBS7/C3/Pr1' (Default 0x00, Register 0x103.6-8)
	  CX25840_SET_EN_12DB_CH1(0x0000)               // 'Disable' (Default 0x00, Register 0x104.0-1)
	  CX25840_SET_EN_12DB_CH2(0x0000)               // 'Disable' (Default 0x00, Register 0x104.1-2)
	  CX25840_SET_EN_12DB_CH3(0x0000)               // 'Disable' (Default 0x00, Register 0x104.2-3)
	  CX25840_SET_HALF_BW_CH1(0x0000)               // '*' (Default 0x00, Register 0x104.3-4)
	  CX25840_SET_HALF_BW_CH2(0x0000)               // '*' (Default 0x00, Register 0x104.4-5)
	  CX25840_SET_HALF_BW_CH3(0x0000)               // '*' (Default 0x00, Register 0x104.5-6)
	  CX25840_SET_VGA_SEL_CH1(0x0000)               // 'Video decoder drives VGA gain setting' (Default 0x00, Register 0x104.6-7)
	  CX25840_SET_VGA_SEL_CH2(0x0000)               // 'Video decoder drives VGA gain setting' (Default 0x00, Register 0x104.7-8)
	  CX25840_SET_VGA_SEL_CH3(0x0000)               // 'Video decoder drives VGA gain setting' (Default 0x00, Register 0x105.0-1)
	  CX25840_SET_CLAMP_SEL_CH1(0x0000)             // 'Video decoder drives clamp level' (Default 0x00, Register 0x105.1-2)
	  CX25840_SET_CLAMP_SEL_CH2(0x0001)             // 'Clamp level is fixed at 3'b111 (midcode clamp). Use for' (Default 0x01, Register 0x105.2-3)
	  CX25840_SET_CLAMP_SEL_CH3(0x0001)             // 'Clamp level is fixed at 3'b111 (midcode clamp). Use for' (Default 0x01, Register 0x105.3-4)
	  CX25840_SET_CHROMA_IN_SEL(0x0001)             // 'ADC 2' (Default 0x01, Register 0x105.4-5)
	  CX25840_SET_LUMA_IN_SEL(0x0000)               // 'ADC 1' (Default 0x00, Register 0x105.5-6)
	  CX25840_SET_AUD_IN_SEL(0x0001)                // 'Miscellaneous Chip Control input' (Default 0x01, Register 0x105.6-7)
	  CX25840_SET_CLAMP_EN_CH1(0x0001)              // 'Enable (power-up)' (Default 0x01, Register 0x105.7-8)
	  CX25840_SET_CLAMP_EN_CH2(0x0000)              // 'Disable' (Default 0x00, Register 0x106.0-1)
	  CX25840_SET_CLAMP_EN_CH3(0x0000)              // 'Disable' (Default 0x00, Register 0x106.1-2)
	  CX25840_SET_DROOP_COMP_CH1(0x0001)            // '*' (Default 0x01, Register 0x106.2-3)
	  CX25840_SET_DROOP_COMP_CH2(0x0000)            // '*' (Default 0x00, Register 0x106.3-4)
	  CX25840_SET_DROOP_COMP_CH3(0x0000)            // '*' (Default 0x00, Register 0x106.4-5)
	  CX25840_SET_BYPASS_CH1(0x0000)                // '*' (Default 0x00, Register 0x106.5-6)
	  CX25840_SET_BYPASS_CH2(0x0000)                // '*' (Default 0x00, Register 0x106.6-7)
	  CX25840_SET_BYPASS_CH3(0x0000)                // '*' (Default 0x00, Register 0x106.7-8)
	  CX25840_SET_IREF_SEL(0x0000)                  // '*' (Default 0x00, Register 0x107.3-4)
	  CX25840_SET_VID_PLL_INT(0x000f)               // '*' (Default 0x0f, Register 0x108.0-6)
	  CX25840_SET_VID_PLL_POST(0x0004)              // '*' (Default 0x04, Register 0x109.0-6)
	  CX25840_SET_AUX_PLL_INT(0x000a)               // '*' (Default 0x0a, Register 0x10a.0-6)
	  CX25840_SET_AUX_PLL_POST(0x0018)              // '*' (Default 0x10, Register 0x10b.0-6)
	  CX25840_SET_AUX_PLL_FRAC1(0x00e5)             // '*' (Default 0x09, Register 0x110.0-8)
	  CX25840_SET_AUX_PLL_FRAC2(0x00d6)             // '*' (Default 0xd7, Register 0x111.0-8)
	  CX25840_SET_AUX_PLL_FRAC3(0x0098)             // '*' (Default 0x98, Register 0x112.0-8)
	  CX25840_SET_AUX_PLL_FRAC4(0x0000)             // '*' (Default 0x00, Register 0x113.0-1)
	  CX25840_SET_SA_MCLK_DIV(0x0010)               // '*' (Default 0x00, Register 0x127.0-6)
	  CX25840_SET_SA_MCLK_SEL(0x0001)               // 'PLL_CLK/PRGM7 is divided by the alternate post-divider' (Default 0x00, Register 0x127.6-7)
	  CX25840_SET_VID_FMT_SEL(0x0000)               // 'AUTO-DETECT' (Default 0x00, Register 0x400.0-4)
	  CX25840_SET_SQ_PIXEL(0x0000)                  // '*' (Default 0x00, Register 0x400.4-5)
	  CX25840_SET_ACFG_DIS(0x0001)                  // 'Disable' (Default 0x00, Register 0x400.5-6)
	  CX25840_SET_AFD_PAL_SEL(0x0000)               // 'PAL-BDGHI' (Default 0x00, Register 0x400.6-7)
	  CX25840_SET_AFD_NTSC_SEL(0x0000)              // 'NTSC-M' (Default 0x00, Register 0x400.7-8)
	  CX25840_SET_AFD_ACQUIRE(0x0000)               // 'The auto-detect state machine operates normally.' (Default 0x00, Register 0x401.0-1)
	  CX25840_SET_INPUT_MODE(0x0001)                // 'Y/C' (Default 0x00, Register 0x401.1-3)
	  CX25840_SET_MAN_SC_FAST_LOCK(0x0000)          // '*' (Default 0x00, Register 0x401.3-4)
	  CX25840_SET_AUTO_SC_LOCK(0x0000)              // 'Manual mode. Lock speed is determined by' (Default 0x01, Register 0x401.4-5)
	  CX25840_SET_CKILLEN(0x0001)                   // 'Enable' (Default 0x01, Register 0x401.5-6)
	  CX25840_SET_CAGCEN(0x0001)                    // 'Enable' (Default 0x01, Register 0x401.6-7)
	  CX25840_SET_WCEN(0x0001)                      // '*' (Default 0x01, Register 0x401.7-8)
	  CX25840_SET_FAST_LOCK_MD(0x0000)              // '*' (Default 0x00, Register 0x402.0-1)
	  CX25840_SET_CLR_LOCK_STAT(0x0000)             // '*' (Default 0x00, Register 0x402.1-2)
	  CX25840_SET_COMB_NOTCH_MODE(0x0001)           // 'Notch data is interpreted as chroma' (Default 0x0a, Register 0x402.2-4)
	  CX25840_SET_CKILL_MODE(0x0000)                // 'Chroma output is forced to 0, and luma output is generated' (Default 0x00, Register 0x402.4-6)
	  CX25840_SET_AFD_PALM_SEL(0x0000)              // 'NTSC will be detected according to the AFD_NTSC_SEL bit.' (Default 0x00, Register 0x403.0-2)
	  CX25840_SET_AFD_FORCE_PAL(0x0000)             // 'The auto-detect algorithm proceeds normally.' (Default 0x00, Register 0x403.2-3)
	  CX25840_SET_AFD_FORCE_PALNC(0x0000)           // 'The auto-detect algorithm proceeds normally.' (Default 0x00, Register 0x403.3-4)
	  CX25840_SET_AFD_FORCE_SECAM(0x0000)           // 'The auto-detect algorithm proceeds normally.' (Default 0x00, Register 0x403.4-5)
	  CX25840_SET_AFD_PAL60_DIS(0x0000)             // 'PAL-60 can be detected and discriminate from NTSC-4.43' (Default 0x00, Register 0x403.5-6)
	  CX25840_SET_AUD_MODE_AUD_SYSTEM(0x000f)       // '*' (Default 0x00, Register 0x808.0-4)
	  CX25840_SET_PREF_MODE(0x0004)                 // '*' (Default 0x00, Register 0x809.0-4)
	  CX25840_SET_MUTE_NO_PREF_MODE(0x0000)         // '*' (Default 0x00, Register 0x809.4-5)
	  CX25840_SET_DE_EMPHASIS_TIME(0x0000)          // '*' (Default 0x00, Register 0x809.5-6)
	  CX25840_SET_FM_DEVIATION(0x0000)              // 'Normal FM' (Default 0x00, Register 0x809.6-8)
	  CX25840_SET_FORMAT_45MHZ(0x0000)              // 'Chroma' (Default 0x00, Register 0x80b.0-3)
	  CX25840_SET_FORMAT_65MHZ(0x0000)              // 'System DK' (Default 0x00, Register 0x80b.3-5)
	  CX25840_SET_TUNER_OUTPUT_FORMAT(0x0000)       // '2nd IF' (Default 0x00, Register 0x80b.5-6)
	  CX25840_SET_SOFT_RESET(0x0000)                // '*' (Default 0x00, Register 0x810.0-1)
	  CX25840_SET_PATH1_AVC_RMS_CON(0x0001)         // '*' (Default 0x49, Register 0x8d0.4-8)
	  CX25840_SET_PATH1_AVC_CR(0x0000)              // '2:1' (Default 0x00, Register 0x8d1.0-3)
	  CX25840_SET_PATH1_AVC_STEREO(0x0000)          // '*' (Default 0x01, Register 0x8d1.3-4)
	  CX25840_SET_PATH1_AVC_AT(0x0001)              // '10 ms' (Default 0x09, Register 0x8d1.4-8)
	  CX25840_SET_PATH1_AVC_RT(0x0001)              // '10 ms' (Default 0x48, Register 0x8d2.0-4)
	  CX25840_SET_PATH1_AVC_CG(0x0000)              // '1' (Default 0x00, Register 0x8d2.4-6)
	  CX25840_SET_SOFT1_MUTE_EN(0x0001)             // 'Enable' (Default 0x01, Register 0x8d3.0-1)
	  CX25840_SET_SRC1_MUTE_EN(0x0000)              // 'Disable' (Default 0x00, Register 0x8d3.1-2)
	  CX25840_SET_SA_MUTE_EN(0x0000)                // 'Disable' (Default 0x00, Register 0x8d3.2-3)
	  CX25840_SET_PAR_MUTE_EN(0x0000)               // 'Disable' (Default 0x00, Register 0x8d3.3-4)
	  CX25840_SET_AC97_MUTE_EN(0x0000)              // 'Disable' (Default 0x00, Register 0x8d3.4-5)
	  CX25840_SET_SRC3_FIFO_RD_TH(0x0008)           // '*' (Default 0x08, Register 0x903.0-4)
	  CX25840_SET_SRC4_PHASE_INC(0x15555)            // '*' (Default 0x14faa, Register 0x904.0-18)
	  CX25840_SET_SRC4_FIFO_RD_TH(0x0008)           // '*' (Default 0x08, Register 0x907.0-4)
	  CX25840_SET_SRC6_PHASE_INC(0x15555)            // '*' (Default 0x14faa, Register 0x90c.0-18)
	  CX25840_SET_SRC6_FIFO_RD_TH(0x0008)           // '*' (Default 0x08, Register 0x90f.0-4)
	  CX25840_EXECUTE
	  
	  CX25840_SET_START_MICROCNTL(0x0000)
	  CX25840_SET_SOFT_RESET(0x0000)
	  CX25840_SET_VD_SOFT_RST(0x0000)
	  CX25840_EXECUTE
	break;
      }
      case CX25840_TUNER: {
	  CX25840_TAG('T','u','n','e','r',' ',cardtype==0?'1':'5',cardtype==0?'5':'0','0')

	  CX25840_SET_START_MICROCNTL(0x0000) // FW stop.
	  CX25840_SET_VD_SOFT_RST(0x0001)     // Assert video reset
	  CX25840_SET_SOFT_RESET(0x0001)      // Assert soft reset
	  CX25840_EXECUTE
			  
	  CX25840_SET_DUAL_MODE_ADC2(0x0000)
	  CX25840_SET_CH_1__SOURCE(cardtype==2?0x0001:0x0006)
	  CX25840_SET_CH_2__SOURCE(0x0003)
	  CX25840_SET_CH_3__SOURCE(cardtype==0?0x0001:0x0000)        // 1 for 150, 0 for 500

	  CX25840_SET_AUX_PLL_POST(0x0018)
	  CX25840_SET_VID_FMT_SEL(0x0000)         // Auto-select video format
	  CX25840_SET_AUD_MODE_AUD_SYSTEM(0x000F) // Force auto-detect of audio
	  CX25840_SET_PREF_MODE(0x0004)           // Preferred audio mode is stereo
	  CX25840_SET_MUTE_NO_PREF_MODE(0x0000)   // Don't mute if preferred output unavailable
	  CX25840_SET_SOFT1_MUTE_EN(0x0001)
	  CX25840_SET_SRC1_MUTE_EN(0x0000)
	  CX25840_SET_SA_MUTE_EN(0x0000)
	  CX25840_SET_PAR_MUTE_EN(0x0001)
	  CX25840_SET_AC97_MUTE_EN(0x0001)
	  CX25840_EXECUTE
  
	  CX25840_SET_SOFT_RESET(0x0000)      // Deassert soft reset
	  CX25840_SET_VD_SOFT_RST(0x0000)     // Deassert video reset
	  CX25840_SET_START_MICROCNTL(0x0001) // FW Start
	  CX25840_EXECUTE
  	  break;
        }
	
	
	case -1:{
		DEBUG(1,"%d is not a valid input!",state->input);
		break;
	}
        }

	// reset audio path	
	cx25840_command(client, DECODER_SET_AUDIO, &state->audio);
      }
    }

  case DECODER_SET_OUTPUT: {
    int *iarg = arg;  
    DEBUG(1, "decoder set output");
    if (*iarg != 0) { return -EINVAL; }
    break;
  }
  case DECODER_ENABLE_OUTPUT: {
    int *iarg = arg;
    int enable = (*iarg != 0);
    
    DEBUG(1, "decoder %s output", enable ? "enable" : "disable");
    
    if (state->enable == enable) break;
   
    state->enable = enable;
      
    if (state->enable) {
	CX25840_TAG('E','n','a','b','l','e') //   Modified regs delta
	
	//CX25840_SET_START_MICROCNTL(0x0000) // FW stop.
	CX25840_SET_SOFT_RESET(0x0001)      // Soft reset assert.
	CX25840_SET_VD_SOFT_RST(0x0001)     // Video assert
	if (no_black_magic == 0) {	
		WI2C(4, 0x43, 0x00,0x16,0x70,0x4a)                // 01063 time 3:37.684
	}
	CX25840_EXECUTE
	
	//CX25840_SET_START_MICROCNTL(0x0001) // FW Start
	CX25840_SET_SOFT_RESET(0x0000)      // Deassert soft reset
	CX25840_SET_VD_SOFT_RST(0x0000)     // Deassert video reset
	CX25840_EXECUTE
    } else {
 
    }
    break;
  }
  case DECODER_GET_WSS: {
    int *iarg = arg;
    u8 status = 0, wss1 = 0, wss2 = 0;
    break; /* FIXME */
    
    
    //status = cx25840_read(client, 0x016b);
    if (status & 0xC0) {
      *iarg = -1;
      break;
    }
    //wss1 = cx25840_read(client, 0x016c);
    //wss2 = cx25840_read(client, 0x016d);
    *iarg = wss2 << 8 | wss1;
    break;
  }
    
  case DECODER_GET_CC_ODD: {
    int *iarg = arg;
    u8 status = 0, cc1 = 0, cc2 = 0;
    
    //status = cx25840_read(client, 0x0166);
    if (status & 0xC0) {
      *iarg = -1;
      break;
    }
    //cc1 = cx25840_read(client, 0x0167);
    //cc2 = cx25840_read(client, 0x0168);
    *iarg = cc2 << 8 | cc1;
    break;
  }
    
  case DECODER_GET_CC_EVEN: {
    int *iarg = arg;
    u8 status = 0, cc1 = 0, cc2 = 0;
    
    //status = cx25840_read(client, 0x0166);
    if (status & 0x30) {
      *iarg = -1;
      break;
    }
    //cc1 = cx25840_read(client, 0x0169);
    //cc2 = cx25840_read(client, 0x016A);
    *iarg = cc2 << 8 | cc1;
    break;
  }
    
  case DECODER_SET_VBI:
    {
      int set = *(int *)arg;
      int is_pal = (state->norm != VIDEO_MODE_NTSC);
      int i;
      u8 lcr[22];
      
      for (i = 2; i <= 23; i++) lcr[i - 2] = 0xFF;
      
      if (set == 0) {
	if (is_pal) for (i = 6; i <= 23; i++) lcr[i - 2] = 0xDD;
	else for (i = 10; i <= 21; i++) lcr[i - 2] = 0xDD;
	for (i = 2; i <= 23; i++) {
	  //cx25840_write(client, 0x0100- 2 + 0x41, lcr[i - 2]);
	}
	// writeregs(client, cfg_cx25840_vbi_on);
	break;
      }
      
      if (set & IVTV_SLICED_TELETEXT_B) {
	if (is_pal) for (i = 6; i <= 22; i++) lcr[i - 2] = 0x11;
	else ERR("Teletext not supported for NTSC");
      }
      if (set & IVTV_SLICED_CAPTION_625) {
	if (is_pal) lcr[22 - 2] = 0x44;
	else ERR("PAL Closed Caption not supported for NTSC");
      }
      if (set & IVTV_SLICED_CAPTION_525) {
	if (!is_pal) {
	  lcr[21 - 2] = 0x44;
	}
	else ERR("NTSC Closed Caption not supported for PAL");
      }
      if (set & IVTV_SLICED_WSS_625) {
	if (is_pal) lcr[23 - 2] = 0x5F;
	else ERR("WSS not supported for NTSC");
      }
      if (set & IVTV_SLICED_VPS) {
	if (is_pal) lcr[16 - 2] = 0x7F;
	else ERR("VPS not supported for NTSC");
      }
      for (i = 2; i <= 23; i++) {
	//cx25840_write(client, 0x0100- 2 + 0x41, lcr[i - 2]);
      }
      // writeregs(client, cfg_cx25840_vbi_off);
      break;
    }

  case DECODER_GET_PICTURE: {
    struct cx25840_state *pic = arg;
    
    pic->bright = state->bright;
    pic->contrast = state->contrast;
    pic->sat = state->sat;
    pic->hue = state->hue;
    break;
  }
    
  case DECODER_SET_PICTURE: {
    struct cx25840_state *pic = arg;
    
    if (pic->bright < 0 || pic->bright > 255) { ERR("invalid brightness setting %d",  pic->bright); return -EINVAL; }
    if (pic->contrast < 0 || pic->contrast > 127) { ERR("invalid contrast setting %d", pic->contrast); return -EINVAL; }
    if (pic->sat < 0 || pic->sat > 127) { ERR("invalid saturation setting %d", pic->sat); return -EINVAL; }
    if (pic->hue < -127 || pic->hue > 127) { ERR("invalid hue setting %d", pic->hue); return -EINVAL; };
    state->contrast = pic->contrast;
    state->bright =pic->bright; 
    state->sat = pic->sat;
    state->hue = pic->hue;

	CX25840_TAG('P','i','c','t','u','r','e')
	CX25840_SET_USAT(state->sat*2)
      	CX25840_SET_VSAT(state->sat*2)
        CX25840_SET_BRIGHT((state->bright-128))
	CX25840_SET_CNTRST(state->contrast*2)
	CX25840_SET_HUE(state->hue)
	CX25840_EXECUTE
    
    DEBUG(1, "decoder set picture bright=%d contrast=%d saturation=%d hue=%d",
	  state->bright, state->contrast, state->sat, state->hue);
    break;
  }
  case DECODER_RESET: {
      CX25840_TAG('R','e','s','e','t')
      CX25840_SET_START_MICROCNTL(0x0000) // FW stop.
      CX25840_SET_SOFT_RESET(0x0001)      // Soft reset assert.
      CX25840_SET_VD_SOFT_RST(0x0001)     // Video assert
      CX25840_EXECUTE
	  
      CX25840_SET_START_MICROCNTL(0x0001) // FW Start
      CX25840_SET_SOFT_RESET(0x0000)      // Deassert soft reset
      CX25840_SET_VD_SOFT_RST(0x0000)     // Deassert video reset
      CX25840_EXECUTE
    break;
  }
  default: 
	  ERR( "invalid ioctl %X", cmd );
	  return -EINVAL;
  }  
  return 0;
}

/* ----------------------------------------------------------------------- */

/* i2c implementation */
#ifndef NEW_I2C
  /* pre i2c-2.8.0 */

static void cx25840_inc_use (struct i2c_client *client) {
#ifdef MODULE
  MOD_INC_USE_COUNT;
#endif
}

static void cx25840_dec_use (struct i2c_client *client) {
#ifdef MODULE
  MOD_DEC_USE_COUNT;
#endif
}

#else
/* i2c-2.8.0 and later */
#endif

#define I2C_CX25840_ADDRESS  0x88

/*
 * Generic i2c probe
 * concerning the addresses: i2c wants 7 bit (without the r/w bit), so '>>1'
 */

static unsigned short normal_i2c[] = { I2C_CX25840_ADDRESS >> 1, I2C_CLIENT_END };
static unsigned short normal_i2c_range[] = { I2C_CLIENT_END };

I2C_CLIENT_INSMOD;

static int cx25840_i2c_id = 0;
struct i2c_driver i2c_driver_cx25840;

static int cx25840_detect_client (struct i2c_adapter *adapter,
				  int                 address,
#ifndef LINUX26                       
				  unsigned short      flags,
#endif                       
				  int                 kind) {
  struct i2c_client *client;
  struct cx25840_state *state;
  int i;  
  if (i2c_enable[cx25840_i2c_id++] == -1) {
    DEBUG(1, "client id: cx25840_i2c_id %d, skipped", cx25840_i2c_id);
    return 0;
  }
  
  DEBUG(1, "detecting client on address 0x%x", address << 1);
  
  /* Check if the adapter supports the needed features */
  if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) return 0;

  client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
  if (client == 0) return -ENOMEM;
  memset(client, 0, sizeof(struct i2c_client));
  client->addr = address;
  client->adapter = adapter;
  client->driver = &i2c_driver_cx25840;
  client->flags = I2C_CLIENT_ALLOW_USE;
  snprintf(client->name, sizeof(client->name), "cx25840");
  
  state = kmalloc(sizeof(struct cx25840_state), GFP_KERNEL);
  i2c_set_clientdata(client, state); 
  if (state == NULL) { kfree(client); return -ENOMEM; }
  memset(state, 0, sizeof(struct cx25840_state));
  state->norm = -1;
  state->input = -1;
  state->enable = 1;
  state->bright = 128;
  state->contrast = 64;
  state->sat = 64;
  state->hue = 0;
  state->audio = DECODER_AUDIO_48_KHZ;
  
  DEBUG(1, "writing init values");
  
  // do_cx25840_dump(client);
  
	for (i = 0; i < 8; i++)
		switch (cardtype) {
			
			case 0:{
				cx25840_input_layout[i] = pvr_150_input_layout[i];
				break;
			} 
			case 1:{
				cx25840_input_layout[i] = pvr_500_input_layout[i];
				break;
			}
			case 2:{
				cx25840_input_layout[i] = pg_600_input_layout[i];
				break;
			}
		}
	

  if( cx25840_initialize(client)<0 )
  {
	  INFO("Failed to initialize %s: Not activating.", client->name );
	  return 0;
  }
			
  
  // FIXME Use this device ID to detect if chip is present...
  DEBUG(1, "Status: cx25%3X (DEVICE_ID)", read_setting(client,DEVICE_ID));
  
  i2c_attach_client(client);	
  log_status(client);
  return 0;
}

static int cx25840_attach_adapter (struct i2c_adapter *adapter) {
  DEBUG(1, "starting probe for adapter %s (0x%x)", adapter->name, adapter->id);
  return i2c_probe(adapter, &addr_data, &cx25840_detect_client);
  return 0;
}

static int cx25840_detach_client (struct i2c_client *client) {
  struct cx25840_state *state = i2c_get_clientdata(client);
  int err;
  
  err = i2c_detach_client(client);
  if (err) { return err; }
  
  kfree(state);
  kfree(client);
  return 0;
}

/* i2c implementation */
struct i2c_driver i2c_driver_cx25840 = {
  .name = "cx25840",
  
  .id = I2C_DRIVERID_CX25840,
  .flags = I2C_DF_NOTIFY,
  
  .attach_adapter = cx25840_attach_adapter,
  .detach_client = cx25840_detach_client,
  .command = cx25840_command,
#ifndef NEW_I2C
  /* pre i2c-2.8.0 */
  .inc_use = cx25840_inc_use,
  .dec_use = cx25840_dec_use,
#else
  /* i2c-2.8.0 and later */
  .owner = THIS_MODULE,
#endif
};

#ifndef LINUX26
EXPORT_NO_SYMBOLS;
#endif

static int __init m__init (void) {
  DEBUG(2, "Module init");
  return i2c_add_driver(&i2c_driver_cx25840);
}

static void __exit m__exit (void) {
  DEBUG(2, "Module exit");
  i2c_del_driver(&i2c_driver_cx25840);
}

module_init(m__init);
module_exit(m__exit);
