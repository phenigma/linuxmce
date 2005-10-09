/*
 * gfxtools.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: gfxtools.c,v 1.7 2004/07/08 10:46:44 schmitzj Exp $
 *
 */

#include <vdr/plugin.h>
#include "gfxtools.h"
#include <string.h>

bool DrawXpm(const char *FileName,areaT *drawable,int x,int y,winhandleT winhand,bool blackwhite)
{ // Bases on vdr cBitmap::LoadXpm but made more readable
  // and added x/y support
	bool Result = false;
	FILE *f = fopen(FileName, "r");
	if (f)
	{
		char **Xpm = NULL;
		bool isXpm = false;
		int lines = 0;
		int index = 0;
		char *s;

		while ((s = readline(f)) != NULL) 
		{
			s = skipspace(s);
			if (!isXpm)
			{
				if (strcmp(s, "/* XPM */") != 0)
				{
					esyslog("ERROR: invalid header in XPM file '%s'", FileName);
					break;
				}
				isXpm = true;
			}
			else if (*s++ == '"')
			{
				if (!lines)
				{
					int w, h, n, c;
					if (4 != sscanf(s, "%d %d %d %d", &w, &h, &n, &c))
					{
						esyslog("ERROR: faulty 'values' line in XPM file '%s'", FileName);
						break;
					}
					lines = h + n + 1;
					Xpm = MALLOC(char *, lines);
				}
				char *q = strchr(s, '"');
				if (!q)
				{
					esyslog("ERROR: missing quotes in XPM file '%s'", FileName);
					break;
				}
				*q = 0;
				if (index < lines)
					Xpm[index++] = strdup(s);
				else
				{
					esyslog("ERROR: too many lines in XPM file '%s'", FileName);
					break;
				}
			}
		}
		if (index == lines)
			Result = DrawXpm(Xpm,drawable,x,y,winhand,blackwhite);
		else
			esyslog("ERROR: too few lines in XPM file '%s'", FileName);
		for (int i = 0; i < index; i++)
			free(Xpm[i]);
		free(Xpm);
		fclose(f);
	}
	else
		esyslog("ERROR: can't open XPM file '%s'", FileName);
	return Result;
}

bool DrawXpm(char *Xpm[], areaT *drawable,int x0,int y0,winhandleT winhand,bool blackwhite)
{ // Bases on vdr cBitmap::LoadXpm but made more readable
  // and added x/y support and "none" is now no longer drawn
	char **p = Xpm;
	int w, h, n, c;
	if (4 != sscanf(*p, "%d %d %d %d", &w, &h, &n, &c))
	{
		esyslog("ERROR: faulty 'values' line in XPM: '%s'", *p);
		return false;
	}
	if (n > MAXNUMCOLORS)
	{
		esyslog("ERROR: too many colors in XPM: %d", n);
		return false;
	}

	int NoneColorIndex = MAXNUMCOLORS;
#if VDRVERSNUM >= 10307
	tColor cols[n];
#else
	eDvbColor cols[n];
#endif
	for (int i = 0; i < n; i++)
	{
		const char *s = *++p;
		if (int(strlen(s)) < c)
		{
			esyslog("ERROR: faulty 'colors' line in XPM: '%s'", s);
			return false;
		}
		s = skipspace(s + c);
		if (*s != 'c')
		{
			esyslog("ERROR: unknown color key in XPM: '%c'", *s);
			return false;
		}
		s = skipspace(s + 1);
		if (strcasecmp(s, "none") == 0)
		{
			s = "#00000000";
			NoneColorIndex = i;
		}
		if (*s != '#')
		{
			esyslog("ERROR: unknown color code in XPM: '%c'", *s);
			return false;
		}
		unsigned int col=strtoul(++s, NULL, 16);
		if (blackwhite)
		{
			int bwcol=(int)(0.299*(double)((col & 0xff0000) >> 16) + 0.587*(double)((col & 0xff00) >> 8) + 0.114*(double)(col & 0xff));
			if (bwcol>0xff) bwcol=0xff;
			bwcol&=(0xff-31);
			col=(bwcol<<16) | (bwcol<<8)| (bwcol);
		}
#if VDRVERSNUM >= 10307
		cols[i] = col | 0xFF000000;
#else
		cols[i] = (eDvbColor)(((col & 0xff) << 16) | (col & 0xff00) | ((col & 0xff0000) >> 16) | 0xFF000000);
#endif
	}
	for (int y = 0; y < h; y++)
	{
		const char *s = *++p;
		if (int(strlen(s)) != w * c)
		{
			esyslog("ERROR: faulty pixel line in XPM: %d '%s'", y, s);
			return false;
		}
		for (int x = 0; x < w; x++)
		{
			for (int i = 0; i < n; i++)
			{
				if (strncmp(Xpm[i + 1], s, c) == 0)
				{
					if (i != NoneColorIndex)
					{
#if VDRVERSNUM >= 10307
						drawable->DrawPixel(x0+x,y0+y, cols[i]);
#else
//						drawable->AddColor(cols[i],winhand);
						drawable->Fill(x0+x,y0+y,x0+x,y0+y,cols[i],winhand);
#endif
					}
					break;
				}
			}
			s += c;
		}
	}
	return true;
}
