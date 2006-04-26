#ifndef WizardPageAudioConnector_H_
#define WizardPageAudioConnector_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"

#include <map>
#include <string>

class WizardPageAudioConnector :
	public WizardPage
{
	std::string OutputValue;

	std::map<std::string, int> Buttons;

	WizardWidgetButton* Selected;

	void SearchSelectedItem();
public:
	virtual int DoApplySetting(SettingsDictionary* Dictionary);
	virtual void DefaultSetup(SettingsDictionary* AVWizardSettings);
	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();

	WizardPageAudioConnector(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageAudioConnector();
};

#endif
