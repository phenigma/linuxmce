#include "WizardWidgetEventManager.h"

WizardWidgetEventManager::WizardWidgetEventManager(WizardWidgetBase* Control)
{
	this->Control = Control;
}

WizardWidgetEventManager::~WizardWidgetEventManager(void)
{
	this->Control = NULL;
}

void WizardWidgetEventManager::SetEvents(SettingsDictionary* Dictionary)
{
	if(Dictionary == NULL)
		return;

}

WizardWidgetEvent* WizardWidgetEventManager::GetEvent(std::string Name)
{
	return CustomEvents[Name];
}
