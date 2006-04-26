/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef ResolutionRefreshScreen_H_ 
#define ResolutionRefreshScreen_H_ 

#include "WizardPage.h"

class ResolutionRefreshScreen :
	public WizardPage
{
public:
	ResolutionRefreshScreen(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~ResolutionRefreshScreen(void);
};

#endif 
