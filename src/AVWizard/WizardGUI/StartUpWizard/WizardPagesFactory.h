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
#define PAGE_VIDEORESOLUTION	3

#define PAGE_UISWITCHER			4

#define PAGE_VIDEOOUTPUT		5
#define PAGE_VIDEOADJUSTSIZE	6
#define PAGE_AUDIOCONNECTOR		7
#define PAGE_AUDIOVOLUME		8
#define PAGE_DOLBYTEST			9
#define PAGE_DTSTEST			10
#define PAGE_FINALSELECTIONS	11

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
	std::string PageNames[11];

public:
	static WizardPagesFactory* GetInstance();
	virtual ~WizardPagesFactory(void);

	void SetRunMode(int RunMode);
	void SetSDLFrontEnd(GenericBackEnd* FrontEnd);

	WizardPage* CreateWizardPage(std::string XMLFile);
	WizardPage* CreatePredefinedWizardPage(int IndexPage);
};

#endif
