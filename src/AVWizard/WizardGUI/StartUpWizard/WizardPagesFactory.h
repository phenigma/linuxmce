/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardPagesFactory_H_
#define WizardPagesFactory_H_

#define WIZARDPAGES_ALL_PAGES 1
#define WIZARDPAGES_ONE_PAGE 2

#include "WizardWidgetsFactory.h"
#include "WizardPage.h"

#include "SDLFrontEnd.h"

#define PAGE_WELCOME			1
#define PAGE_VIDEORATIO			2
#define PAGE_VIDEOOUTPUT		3
#define PAGE_VIDEORESOLUTION	4
#define PAGE_VIDEOADJUSTSIZE	5
#define PAGE_AUDIOCONNECTOR		6
#define PAGE_AUDIOVOLUME		7
#define PAGE_DOLBYTEST			8
#define PAGE_DTSTEST			9
#define PAGE_FINALSELECTIONS	10

class WizardPagesFactory : public WizardWidgetsFactory
{
	static WizardPagesFactory* Instance;
	WizardPagesFactory(void);

	int RuningMode;

	WizardPage* CreateWidget(SettingsDictionaryTree* 
		DialogSource);

	/**
	 *	Fixed names for XML based layouts for pages
	 */
	std::string PageNames[10];

public:
	static WizardPagesFactory* GetInstance();
	virtual ~WizardPagesFactory(void);

	void SetRunMode(int RunMode);
	void SetSDLFrontEnd(SDLFrontEnd* FrontEnd);

	WizardPage* CreateWizardPage(std::string XMLFile);
	WizardPage* CreatePredefinedWizardPage(int IndexPage);
};

#endif
