#ifndef WizardPagesFactory_H_
#define WizardPagesFactory_H_

#define WIZARDPAGES_ALL_PAGES 1
#define WIZARDPAGES_ONE_PAGE 2

#include "WizardWidgetsFactory.h"
#include "WizardPage.h"

#include "SDLFrontEnd.h"

#define PAGE_VIDEORATIO			1
#define PAGE_VIDEOOUTPUT		2
#define PAGE_VIDEORESOLUTION	3
#define PAGE_AUDIOCONNECTOR		4
#define PAGE_AUDIOVOLUME		5
#define PAGE_DOLBYTEST			6
#define PAGE_DTSTEST			7
#define PAGE_FINALSELECTIONS	8

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
	std::string PageNames[8];

public:
	static WizardPagesFactory* GetInstance();
	virtual ~WizardPagesFactory(void);

	void SetRunMode(int RunMode);
	void SetSDLFrontEnd(SDLFrontEnd* FrontEnd);

	WizardPage* CreateWizardPage(std::string XMLFile);
	WizardPage* CreatePredefinedWizardPage(int IndexPage);
};

#endif
