/*
 * gfxtools.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: gfxtools.h,v 1.6 2004/07/07 15:14:23 schmitzj Exp $
 *
 */

#ifndef GFXTOOLS_H
#define GFXTOOLS_H

#include <vdr/plugin.h>
#include "anyfont.h"

#if VDRVERSNUM >= 10307
typedef cOsd areaT;
typedef int winhandleT;
#else
typedef cOsdBase areaT;
typedef tWindowHandle winhandleT;
#endif

bool DrawXpm(const char *FileName,areaT *drawable,int x0,int y0,winhandleT winhand=0,bool blackwhite=false);
bool DrawXpm(char *Xpm[], areaT *drawable,int x0,int y0,winhandleT winhand=0,bool blackwhite=false);

#endif
