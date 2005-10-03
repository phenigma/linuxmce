/*
 *  $Id: skin.c,v 1.3 2005/01/01 23:44:36 lordjaxom Exp $
 */

#include "xml/skin.h"
#include "i18n.h"
#include <vdr/tools.h>
#include <vdr/config.h>

const std::string ScreenBases[] = { "relative", "absolute" };

cxSkin::cxSkin(const std::string &Name, cText2SkinI18n *I18n, cText2SkinTheme *Theme):
		mName(Name),
		mI18n(I18n),
		mTheme(Theme) 
{
}

void cxSkin::SetBase(eScreenBase Base) 
{
	if (Base != (eScreenBase)-1)
		mBase = Base;

	switch (mBase) {
	case relative:
		mBaseOffset = txPoint(Setup.OSDLeft, Setup.OSDTop);
		mBaseSize   = txSize(Setup.OSDWidth, Setup.OSDHeight);
		break;

	case absolute:
		mBaseOffset = txPoint(0, 0);
		mBaseSize   = txSize(720, 576); //XXX
		break;

	default:
		break;
	}
}

bool cxSkin::ParseBase(const std::string &Text) 
{
	int i;
	for (i = 0; i < (int)__COUNT_BASE__; ++i) {
		if (ScreenBases[i] == Text)
			break;
	}
	if (i < (int)__COUNT_BASE__) {
		SetBase((eScreenBase)i);
		return true;
	}
	return false;
}
	
std::string cxSkin::Translate(const std::string &Text)
{
	if (mI18n != NULL)
		return mI18n->Translate(Text);
	return Text;
}
