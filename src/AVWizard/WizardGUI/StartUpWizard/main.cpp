#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//---------------------------------------------------------------------------

#include "Wizard.h"
//---------------------------------------------------------------------------
#include "PageFileParser.h"
//---------------------------------------------------------------------------
#include "WizardWidgetPage.h"
#include "WizardWidgetsFactory.h"
//---------------------------------------------------------------------------
#include "GenerateWizardConfigDefaults.h"
//---------------------------------------------------------------------------
bool IsConsole = false;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{

	GenerateWizardConfigDefaults Generator;
	Generator.GenerateDefaults();
	Generator.GenerateDefaultPages("", "wiz_pixmaps");

	Wizard* m_Wizard = Wizard::GetInstance();
	m_Wizard->ParseCommandLineParameters(argc, argv);
	m_Wizard->StartSDLVideoMode();
	m_Wizard->CreateDialogs();
	m_Wizard->MainLoop();	
	m_Wizard->CleanUp();

	return 0;
}
