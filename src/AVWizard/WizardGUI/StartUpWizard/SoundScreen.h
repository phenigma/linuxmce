/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef SoundScreen_H_
#define SoundScreen_H_

#include "WizardPage.h"

class SoundScreen :
	public WizardPage
{
public:
	SoundScreen(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~SoundScreen(void);
};

#endif
