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
