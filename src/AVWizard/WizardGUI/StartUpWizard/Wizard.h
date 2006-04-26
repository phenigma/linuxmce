/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef Wizard_H_
#define Wizard_H_


#ifdef WIN32
#define LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <atlbase.h>
#else
#include <signal.h>
#endif

#include "SDLFrontEnd.h"
#include "ScreenManager.h"

#define Wizard_Default_Width 640
#define Wizard_Default_Height 480

#define WIZARD_CHANGE_RESOLUTION 1
#define WIZARD_CHANGE_REFRESH 2

#define WIZARD_SELECTED_FONT_SIZE		50
#define WIZARD_SELECTED_LABEL_SIZE		30
#define WIZARD_DESELECTED_FONT_SIZE		30
#define WIZARD_DESELECTED_LABEL_SIZE	20

#ifdef WIN32
#define snprintf _snprintf
#endif


/**
 *	For now may change only the resolution and the refresh
 */
#include "WizardPage.h"

#include "SettingsDictionaryTree.h"

/**
 *	Singleton class that consist all the wizard, is singleton because 
 *	is the core class of the AVWizard
 */
class Wizard
{

	/**
	 *	Current video mode that runs the wizard
	 */
	int Width;
	int Height;
	bool FullScreen;

	/**
	 *	SettingsDictionary that keeps the options of command line
	 */
	SettingsDictionary CommandLineOptions;

	/**
	 *	Index to current page in the wizard
	 */
	int CurrentPage;
	/**
	 *	The current page of the wizard
	 */
	WizardPage* MainPage;
	
	/**
	 * Quit attribute is true if the application is pending to quit
	 */
	bool Quit;
	/**
	 * The actual application should display everything using a SDL application
	 */
	SDLFrontEnd* FrontEnd;
	
	/**
	 * That object is used for changing the display modes
	 */
	ScreenManager Manager;
	
	/**
	 * Last pending event definition
	 */
	WM_Event Event;
	/**
	 * Treat the current keys and window events
	 */
	void TreatEvents();
	

	bool StatusChange;

	int WizardChangeType;
	/**
	 * Make the decrease action, depends of the 
	 * WizardChangeType mode
	 */
	void DoDecreaseAction();
	/**
	 * Make the increase action, depends of the 
	 * WizardChangeType mode
	 */
	void DoIncreaseAction();
	/**
	 * Change WizardChangeType mode to the previous one
	 */
	void DoChangeActionBefore();
	/**
	 * Change WizardChangeType mode to the next one
	 */
	void DoChangeActionAfter();
	/**
	 *	Apply the current settings from the current screen
	 */
	void DoApplyScreen(SettingsDictionary* Settings);
	/**
	 *	Apply the current settings from the current screen
	 */
	void DoCancelScreen();
	
	void EvaluateEvent(WM_Event& Event);

	void PaintStatus();

	/**
	 *	Private constructor, because is a signleton class	
	 */
	Wizard();

	static Wizard* Instance;

public:
	/**
	 *	SettingsDictionary that keep global AVWizard Settings
	 */
	SettingsDictionaryTree* AVWizardOptions;

	virtual ~Wizard();
	static Wizard* GetInstance();
	void CleanUp();

	int ParseCommandLineParameters(int argc, char** argv);

	void StartSDLVideoMode();

	void Configure();

	void MainLoop();

	/**
	 *	Generate an custom event
	 */
	void GenerateCustomEvent(WM_Event Event);

	void CreateDialogs();

};

#endif /*Wizard_H_*/
