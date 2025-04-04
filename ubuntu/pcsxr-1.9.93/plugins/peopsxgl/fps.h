/***************************************************************************
                          fps.h  -  description
                             -------------------
    begin                : Sun Mar 08 2009
    copyright            : (C) 1999-2009 by Pete Bernert
    web                  : www.pbernert.com   
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

void InitFrameCap(void);
void SetFrameRateConfig(void);
void PCFrameCap(void);
void PCcalcfps(void);
void FrameSkip(void);
void CheckFrameRate(void);
void ReInitFrameCap(void);
void SetAutoFrameCap(void);

#ifndef _WINDOWS
unsigned long timeGetTime();
#endif
