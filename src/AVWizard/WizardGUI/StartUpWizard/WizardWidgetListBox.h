/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//--------------------------------------------------------------
#ifndef WizardWidgetListBox_H_
#define WizardWidgetListBox_H_
//---------------------------------------------------------------------------
#include "WizardWidgetButton.h"
#include "WizardWidgetPage.h"
#include <SDL.h>
//---------------------------------------------------------------------------
class WizardWidgetListBox : public WizardWidgetButton
{
	/**
	 *	The containing text of the button	
	 */
	std::string Caption;

	bool DisplayLeftArrow, DisplayRightArrow;

	void SetSurface(std::string FileName, int IndexSurface);


public:
	WizardWidgetListBox(SDLFrontEnd* FrontEnd, std::string Name);
	~WizardWidgetListBox();

	/**
	 *	Paints the control and the children of the curent component
	 */
	virtual void Paint();
	/**
	 *	Set up the known attributes
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);
	/**
	 *	Say if listbox has focus or doesn't
	 */
	virtual void SetFocus(bool IsFocused);

	virtual void DisplayArrows(bool Left, bool Right);

};

#endif
