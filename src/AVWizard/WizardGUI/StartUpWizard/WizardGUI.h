#ifndef WIZARD_GUI_H
#define  WIZARD_GUI_H

#include "WizardPage.h"

class WizardGUI
{
	WizardPage* CurrentPage;
public:
	WizardGUI(void);
	virtual ~WizardGUI(void);

	bool LoadSettings(char* FileName);
	bool SaveSettings(char* FileName);

	bool SwitchPreviousScreen();
	bool SaveSettings();
};

#endif