/*
 * $Id: menu.c,v 1.1.1.1 2004/12/19 22:03:14 lordjaxom Exp $
 */

#include "menu.h"
#include "bitmap.h"

// --- cText2SkinSetupPage ----------------------------------------------------

cText2SkinSetupPage::cText2SkinSetupPage(void) {
	mData = Text2SkinSetup;
	Add(new cOsdItem(tr("Flush image cache"), osUser1));
	Add(new cMenuEditIntItem(tr("Max. image cache size"), &mData.MaxCacheFill));
}

cText2SkinSetupPage::~cText2SkinSetupPage() {
}

void cText2SkinSetupPage::Store(void) {
	SetupStore("MaxCacheFill", mData.MaxCacheFill);
	Text2SkinSetup = mData;
}

eOSState cText2SkinSetupPage::ProcessKey(eKeys Key) {
	eOSState state = cMenuSetupPage::ProcessKey(Key);
	if (state == osUser1) {
		Skins.Message(mtInfo, tr("Flushing image cache..."));
		cText2SkinBitmap::FlushCache();
		Skins.Message(mtInfo, NULL);
		return osContinue;
	}
	return state;
}

