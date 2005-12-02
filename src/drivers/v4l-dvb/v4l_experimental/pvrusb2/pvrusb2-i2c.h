/*
 *
 *  $Id: pvrusb2-i2c.h,v 1.1 2005/11/14 13:31:24 mchehab Exp $
 *
 *  Copyright (C) 2005 Mike Isely <isely@pobox.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __PVRUSB2_I2C_H
#define __PVRUSB2_I2C_H

struct pvr2_hdw;

void pvr2_i2c_init(struct pvr2_hdw *);
void pvr2_i2c_done(struct pvr2_hdw *);
int pvr2_i2c_tuner_cmd(struct pvr2_hdw *,unsigned int,void *);
int pvr2_i2c_msp3400_cmd(struct pvr2_hdw *,unsigned int,void *);
int pvr2_i2c_saa7115_cmd(struct pvr2_hdw *,unsigned int,void *);
int pvr2_i2c_tveeprom_cmd(struct pvr2_hdw *,unsigned int,void *);

#endif /* __PVRUSB2_I2C_H */


/*
  Stuff for Emacs to see, in order to encourage consistent editing style:
  *** Local Variables: ***
  *** mode: c ***
  *** fill-column: 75 ***
  *** tab-width: 8 ***
  *** c-basic-offset: 8 ***
  *** End: ***
  */
