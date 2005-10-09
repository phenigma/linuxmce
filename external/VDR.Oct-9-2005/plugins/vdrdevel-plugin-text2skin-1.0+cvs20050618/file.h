/*
 * $Id: file.h,v 1.1.1.1 2004/12/19 22:03:13 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_FILE_H
#define VDR_TEXT2SKIN_FILE_H

#include "common.h"
#include <vdr/tools.h>

class cText2SkinFile {
private:
	std::string mSkin;

protected:
	virtual bool Parse(const char *Text) = 0;

public:
	cText2SkinFile(const char *Skin);
	virtual ~cText2SkinFile();

	virtual bool Load(const std::string &Filename);
	
	const std::string &Skin(void) const { return mSkin; }
};

#endif // VDR_TEXT2SKIN_FILE_H

