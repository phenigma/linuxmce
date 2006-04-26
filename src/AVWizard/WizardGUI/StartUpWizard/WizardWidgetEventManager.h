//---------------------------------------------------------------------------
#ifndef WizardWidgetEventManager_H_
#define WizardWidgetEventManager_H_
//---------------------------------------------------------------------------
#include "DelegatesEvent.h"
#include <map>
//---------------------------------------------------------------------------
#include "SettingsDictionary.h"
//---------------------------------------------------------------------------
class WizardWidgetBase;
//---------------------------------------------------------------------------
typedef DelegatesEvent<void, void*> WizardWidgetEvent;
//---------------------------------------------------------------------------
/**
 *	WizardWidgetEventManager - class that consists in the XML assigned events 
 *	to a control
 */
class WizardWidgetEventManager
{
	WizardWidgetBase* Control;
	std::map<std::string, WizardWidgetEvent*> CustomEvents;

public:
	WizardWidgetEventManager(WizardWidgetBase* Control);
	virtual ~WizardWidgetEventManager();

	void SetEvents(SettingsDictionary* Dictionary);

	WizardWidgetEvent* GetEvent(std::string Name);
};
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
