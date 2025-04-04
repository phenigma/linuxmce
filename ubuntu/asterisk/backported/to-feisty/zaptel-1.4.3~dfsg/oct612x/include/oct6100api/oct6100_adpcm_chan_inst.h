/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*\

File:  oct6100_adpcm_chan_inst.h

    Copyright (c) 2001-2006 Octasic Inc.
    
Description: 

	File containing all defines, macros, and structures pertaining to the file
	oct6100_adpcm_chan.c.  All elements defined in this file are for public
	usage of the API.  All private elements are defined in the
	oct6100_adpcm_chan_priv.h file.

This file is part of the Octasic OCT6100 GPL API . The OCT6100 GPL API  is 
free software; you can redistribute it and/or modify it under the terms of 
the GNU General Public License as published by the Free Software Foundation; 
either version 2 of the License, or (at your option) any later version.

The OCT6100 GPL API is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
for more details. 

You should have received a copy of the GNU General Public License 
along with the OCT6100 GPL API; if not, write to the Free Software 
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.

$Octasic_Release: OCT612xAPI-01.00-PR47 $

$Octasic_Revision: 6 $

\*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/

#ifndef __OCT6100_ADPCM_CHAN_INST_H__
#define __OCT6100_ADPCM_CHAN_INST_H__

/*****************************  INCLUDE FILES  *******************************/

/*****************************  DEFINES  *************************************/

/*****************************  TYPES  ***************************************/

typedef struct _OCT6100_API_ADPCM_CHAN_
{
	/* Flag specifying whether the entry is used or not. */
	UINT8	fReserved;
	
	/* Count used to manage entry handles allocated to user. */
	UINT8	byEntryOpenCnt;

	/* TSI chariot memory entry. */
	UINT16	usTsiMemIndex;
	
	/* ADPCM memory entry. */
	UINT16	usAdpcmMemIndex;

	/* Input and output timeslot information. */
	UINT16	usInputTimeslot;
	UINT16	usInputStream;
	UINT8	byInputNumTssts;
	UINT8	byInputPcmLaw;

	UINT16	usOutputTimeslot;
	UINT16	usOutputStream;
	UINT8	byOutputNumTssts;
	UINT8	byOutputPcmLaw;

	/* Internal info for quick access to structures associated to this TSI cnct. */
	UINT16	usInputTsstIndex;
	UINT16	usOutputTsstIndex;

} tOCT6100_API_ADPCM_CHAN, *tPOCT6100_API_ADPCM_CHAN;

#endif /* __OCT6100_ADPCM_CHAN_INST_H__ */
