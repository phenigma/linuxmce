#ifndef Wizard_H_
#define Wizard_H_

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

/**
 *	For now may change only the resolution and the refresh
 */
#define WIZARD_NO_POSSIBLE_ACTIONS 2

#include "WizardGUI.h"

class Wizard
{
	/**
	 *	The current page of the wizard
	 */
	WizardGUI Pages;
	
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
	
	void EvaluateEvent(WM_Event& Event);

	void PaintStatus();

public:
	
	Wizard();
	virtual ~Wizard();
	
	void StartSDLVideoMode(int Width, int Height, bool FullScreen);
	
	void MainLoop();
};

#endif /*Wizard_H_*/
