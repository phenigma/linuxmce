#include <stdio.h>
#include <stdlib.h>
#include <iostream>
//---------------------------------------------------------------------------
#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <atlbase.h>
#else
#include <signal.h>
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
#ifndef WIN32
void signal_handler(int signal)
{
	switch (signal)
	{
		case SIGUSR1:
			exit(2);
			break;
	}
}
#endif
//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
#ifndef WIN32
	signal(SIGUSR1, signal_handler);
#endif
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
