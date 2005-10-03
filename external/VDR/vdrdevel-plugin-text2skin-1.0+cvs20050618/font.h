/*
 * $Id: font.h,v 1.2 2004/12/28 14:35:54 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_FONT_H
#define VDR_TEXT2SKIN_FONT_H

#include "common.h"
#ifdef HAVE_FREETYPE
#	include "graphtft/font.h"
#endif

class cText2SkinFont {
private:
#ifdef HAVE_FREETYPE
	static cGraphtftFont mFontCache;
#endif

	// disallow direct construction
	cText2SkinFont(void);
	virtual ~cText2SkinFont();

public:
	static const cFont *Load(const std::string &Path, const std::string &Filename, int Size, 
	                         int Width);
};

#endif // VDR_TEXT2SKIN_FONT_H
