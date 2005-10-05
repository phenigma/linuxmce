/*
    wm8775 API header
    Copyright (C) 2004 Ulf Eklund

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

typedef enum {
  RESET=0, TOD, ADCHPD, ADCMCLK, ADCWL, ADCBCP, ADCLRP, ADCFMT, ADCMS, ADCOSR, ADCRATE, AINPD, ADCPD, PWDN, ZCLA, LAG, ZCRA, RAG, 
  LCSEL, MAXGAIN, LCT, LCEN, ALCZC, HLD, DCY, ATK, NGTH, NGAT, TRANWIN, MAXATTEN, LRBOTH, MUTELA, MUTERA, AMX 
} wm8775_setting_id;

typedef struct {
  __u16 setting_id;
  __u16 value;
} config_request;

typedef struct {
  int            list_length;
  config_request list[1];
} wm8775_ioctl;

#define WM8775_CONFIGURE _IOW( 'w', 100, wm8775_ioctl)
#define WM8775_LOG_STATE _IO(  'w', 101)
#define WM8775_GET_STATE _IOWR('w', 102, wm8775_ioctl)
