/*
 * $Id: ir-common.h,v 1.7 2005/02/16 13:11:55 kraxel Exp $
 *
 * some common structs and functions to handle infrared remotes via
 * input layer ...
 *
 * (c) 2003 Gerd Knorr <kraxel@bytesex.org> [SuSE Labs]
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/version.h>
#include <linux/input.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
/* some keys from 2.6.x which are not (yet?) in 2.4.x */
# define KEY_PLAY                207
# define KEY_PRINT	     	 210
# define KEY_SEARCH              217
# define KEY_SELECT 	         0x161
# define KEY_GOTO                0x162
# define KEY_INFO                0x166
# define KEY_CHANNEL             0x16b
# define KEY_LANGUAGE            0x170
# define KEY_SUBTITLE	         0x172
# define KEY_ZOOM                0x174
# define KEY_MODE		 0x175
# define KEY_TV                  0x179
# define KEY_CD                  0x17f
# define KEY_TUNER               0x182
# define KEY_TEXT                0x184
# define KEY_DVD		 0x185
# define KEY_AUDIO               0x188
# define KEY_VIDEO               0x189
# define KEY_RED                 0x18e
# define KEY_GREEN               0x18f
# define KEY_YELLOW              0x190
# define KEY_BLUE                0x191
# define KEY_CHANNELUP           0x192
# define KEY_CHANNELDOWN         0x193
# define KEY_RESTART	         0x198
# define KEY_SHUFFLE     	 0x19a
#endif

#define IR_TYPE_RC5     1
#define IR_TYPE_OTHER  99

#define IR_KEYTAB_TYPE	u32
#define IR_KEYTAB_SIZE	128  // enougth for rc5, probably need more some day ...

#define IR_KEYCODE(tab,code)	(((unsigned)code < IR_KEYTAB_SIZE) \
				 ? tab[code] : KEY_RESERVED)

struct ir_input_state {
	/* configuration */
	int                ir_type;
	IR_KEYTAB_TYPE     ir_codes[IR_KEYTAB_SIZE];

	/* key info */
	u32                ir_raw;      /* raw data */
	u32                ir_key;      /* ir key code */
	u32                keycode;     /* linux key code */
	int                keypressed;  /* current state */
};

extern IR_KEYTAB_TYPE ir_codes_rc5_tv[IR_KEYTAB_SIZE];
extern IR_KEYTAB_TYPE ir_codes_empty[IR_KEYTAB_SIZE];
extern IR_KEYTAB_TYPE ir_codes_hauppauge_new[IR_KEYTAB_SIZE];

void ir_input_init(struct input_dev *dev, struct ir_input_state *ir,
		   int ir_type, IR_KEYTAB_TYPE *ir_codes);
void ir_input_nokey(struct input_dev *dev, struct ir_input_state *ir);
void ir_input_keydown(struct input_dev *dev, struct ir_input_state *ir,
		      u32 ir_key, u32 ir_raw);
u32  ir_extract_bits(u32 data, u32 mask);
int  ir_dump_samples(u32 *samples, int count);
int  ir_decode_biphase(u32 *samples, int count, int low, int high);

/*
 * Local variables:
 * c-basic-offset: 8
 * End:
 */
