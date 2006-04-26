#ifndef WizardPageFinalSelections_H_
#define WizardPageFinalSelections_H_

#include "WizardPage.h"

#include "WizardWidgetButton.h"
#include <map>
#include <string>

class WizardPageFinalSelections :
	public WizardPage
{
	std::map <std::string, int> Buttons;
	WizardWidgetButton* Selected;
	std::string OutputValue;

	std::string GetVideoRatio(SettingsDictionary* AVWizardSettings);
	std::string GetVideoOutput(SettingsDictionary* AVWizardSettings);
	std::string GetVideoResolution(SettingsDictionary* AVWizardSettings);
	std::string GetVideoRefresh(SettingsDictionary* AVWizardSettings);
	std::string GetAudioConnector(SettingsDictionary* AVWizardSettings);
	std::string GetVolume(SettingsDictionary* AVWizardSettings);
	std::string GetDolbyTestStatus(SettingsDictionary* AVWizardSettings);
	std::string GetDTSTestStatus(SettingsDictionary* AVWizardSettings);
public:
	virtual void DoIncreaseSetting();
	virtual void DoDecreaseSetting();

	virtual void SearchSelectedItem();

	WizardPageFinalSelections(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardPageFinalSelections(void);

	void DefaultSetup(SettingsDictionary* AVWizardSettings);
	virtual int DoApplySetting(SettingsDictionary* Dictionary);
};

#endif
