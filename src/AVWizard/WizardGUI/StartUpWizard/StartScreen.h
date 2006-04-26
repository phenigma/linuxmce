#ifndef StartScreen_H_
#define StartScreen_H_

#include "WizardPage.h"

class StartScreen :
	public WizardPage
{
public:
	StartScreen(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~StartScreen(void);
};

#endif
