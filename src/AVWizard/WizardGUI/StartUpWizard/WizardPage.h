/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
#ifndef WizardPage_H_
#define WizardPage_H_

#include "WizardWidgetPage.h"

#include <vector>

class Wizard;

class WizardPage 
{
protected:
	std::vector<WizardWidgetBase*> FocusableControls;
	std::vector<WizardWidgetBase*>::iterator FocusControl;

	SDLFrontEnd* FrontEnd;
	std::string Name;
	/**
	 *	Builds the list with focusable items
	 */
	void FindFocusableControlsRecursive(WizardWidgetBase* RootWidget);
	/**
	 *	Widget that describes the wizard layout
	 */
	WizardWidgetPage* Page;
	Wizard* MainWizard;

	/**
	*	Must override function, used to get setup defaults of the dialog from the wizard
	*/
	virtual void WizardPage::DefaultSetup(SettingsDictionary* AVWizardSettings);
public:
	/**
	 * Default constructor
	 */
	WizardPage(SDLFrontEnd* FrontEnd, std::string Name);

	/**
	 * Default destructor
	 */
	virtual ~WizardPage();
	
	WizardWidgetPage* GetPageLayout();

	/**
	 *    Gives a layout description to the current page
	 * @param PageDescription 
	 */
	void SetPageLayout(WizardWidgetPage* PageDescription);

	/**
	 *	Similar with pressing on current item the down arrow
	 */
	virtual void DoNextFocusItem();

	/**
	 *	Similar with pressing on current item the up arrow
	 */
	virtual void DoPreviousFocusItem();

	/**
	 *	Simiar with pressing on current item the right arrow
	 */
	virtual void DoIncreaseSetting();

	/**
	 *	Simiar with pressing on current item the left arrow
	 */
	virtual void DoDecreaseSetting();

	/**
	 *	Similar with pressing on current item the Enter key
	 * @param Dictionary The place where stores curretn values
	 * @return Return a negative value if it wants to ignore that command
	 */
	virtual int DoApplySetting(SettingsDictionary* Dictionary);

	/**
	 *	Similar with pressing on current item the Escape key
	 */
	virtual void DoCancelSetting();

	/**
	 *	Builds the list with focusable items
	 */
	void FindFocusableControls();

	/**
	 *	Get the MainWizard reference to notify the Wizard on change screen
	 */
	void SetWizard(Wizard* MainWizard);

};

#endif
