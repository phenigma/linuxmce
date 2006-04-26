#include <stdio.h>
#include <iostream>
//---------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <atlbase.h>
#endif

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
