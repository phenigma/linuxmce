/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */

#ifndef SaveSettingsScreen_H_
#define SaveSettingsScreen_H_

#include "WizardPage.h"

class SaveSettingsScreen :
	public WizardPage
{
public:
	SaveSettingsScreen(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~SaveSettingsScreen(void);
};

#endif
