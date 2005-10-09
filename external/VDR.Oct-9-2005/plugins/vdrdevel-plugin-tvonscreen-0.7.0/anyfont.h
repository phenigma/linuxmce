/*
 * anyfont.h: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: anyfont.h,v 1.2 2004/07/06 13:02:14 schmitzj Exp $
 *
 */

#ifndef ANYFONT_H
#define ANYFONT_H

#include <vdr/plugin.h>

class anyFont
{
#if VDRVERSNUM >= 10307
	cOsd *osd;
#else
	cOsdBase *osd;
#endif
	const cFont::tPixelData *FontData;
	int FontHeight;
	int trans;
public:
#if VDRVERSNUM >= 10307
	anyFont(cOsd *o,const cFont::tPixelData *fd,int fheight,int transparent=0);
#else
	anyFont(cOsdBase *o,const cFont::tPixelData *fd,int fheight,int transparent=0);
#endif
	int Height(void);
	int Width(const char *txt);
	int LargeWidth(const char *txt);
	int Width(char c);
	int LargeWidth(char c);
#if VDRVERSNUM >= 10307
	int Text(int x, int y, const char *txt, tColor fg, tColor bg);
	int LargeText(int x, int y, const char *txt, tColor fg, tColor bg);

	int Text(int x, int y, int w, int h, const char *txt, tColor fg, tColor bg);
#else
	int Text(int x, int y, const char *txt, eDvbColor fg, eDvbColor bg, tWindowHandle wh);
	int LargeText(int x, int y, const char *txt, eDvbColor fg, eDvbColor bg, tWindowHandle wh);

	int Text(int x, int y, int w, int h, const char *txt, eDvbColor fg, eDvbColor bg, tWindowHandle wh);
#endif
	int TextHeight(int w, const char *txt);
};

#endif
