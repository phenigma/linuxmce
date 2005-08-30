/*
 *  $Id: skin.h,v 1.4 2005/01/17 18:52:52 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_XML_SKIN_H 
#define VDR_TEXT2SKIN_XML_SKIN_H 

#include "xml/display.h"
#include <vdr/osd.h>
#include <map>
#include <string>

// --- cxSkin -----------------------------------------------------------------

class cText2SkinI18n;
class cText2SkinTheme;

class cxSkin {
	friend bool xStartElem(const std::string &name, std::map<std::string,std::string> &attrs);
	friend bool xEndElem(const std::string &name);

	/* Skin Editor */
	friend class VSkinnerView;

public:
	enum eScreenBase {
		relative,
		absolute,
#define __COUNT_BASE__ (absolute + 1)
	};

private:
	eScreenBase      mBase;
	txPoint          mBaseOffset;
	txSize           mBaseSize;
	std::string      mName;
	std::string      mTitle;
	std::string      mVersion;
	
	cxDisplays       mDisplays;
	
	cText2SkinI18n  *mI18n; // TODO: should move here completely
	cText2SkinTheme *mTheme;

public:
	cxSkin(const std::string &Name, cText2SkinI18n *I18n, cText2SkinTheme *Theme);

	cxDisplay *Get(cxDisplay::eType Type);

	bool ParseBase(const std::string &Text);
	void SetBase(eScreenBase Base = (eScreenBase)-1);

	eScreenBase        Base(void)       const { return mBase; }
	const txPoint     &BaseOffset(void) const { return mBaseOffset; }
	const txSize      &BaseSize(void)   const { return mBaseSize; }
	const std::string &Name(void)       const { return mName; }
	const std::string &Title(void)      const { return mTitle; }
	const std::string &Version(void)    const { return mVersion; }

	// functions for object classes to obtain dynamic item information
	std::string        Translate(const std::string &Text);
};

inline cxDisplay *cxSkin::Get(cxDisplay::eType Type) {
	if (mDisplays.find(Type) != mDisplays.end())
		return mDisplays[Type];
	return NULL;
}

#endif // VDR_TEXT2SKIN_XML_SKIN_H 
