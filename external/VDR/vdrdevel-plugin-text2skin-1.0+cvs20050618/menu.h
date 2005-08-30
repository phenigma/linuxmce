/*
 * $Id: menu.h,v 1.1.1.1 2004/12/19 22:03:14 lordjaxom Exp $
 */

#ifndef VDR_TEXT2SKIN_MENU_H
#define VDR_TEXT2SKIN_MENU_H

#include "common.h"
#include "setup.h"
#include <vdr/menuitems.h>

class cText2SkinSetupPage: public cMenuSetupPage {
private:
	cText2SkinSetup mData;

public:
	cText2SkinSetupPage(void);
	virtual ~cText2SkinSetupPage();

	virtual void Store(void);
	eOSState ProcessKey(eKeys Key);
};

#endif // VDR_TEXT2SKIN_MENU_H
