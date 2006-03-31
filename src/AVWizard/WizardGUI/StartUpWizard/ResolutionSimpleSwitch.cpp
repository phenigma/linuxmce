// ResolutionSimpleSwitch.cpp : Defines the entry point for the console application.
//


#include "Wizard.h"
#include "WizardSettings.h"


int main(int argc, char* argv[])
{
/*	Wizard m_Wizard;

	m_Wizard.StartSDLVideoMode(640, 480, false);	

	m_Wizard.MainLoop();	
*/
	WizardSettings Settings;

	Settings.SaveSettingsToXMLFile("options.xml");
	Settings.LoadSettingsFromXMLFile("options.xml");

	return 0;
}
