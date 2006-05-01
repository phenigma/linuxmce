/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//--------------------------------------------------------------
#ifndef WizardWidgetButton_H_
#define WizardWidgetButton_H_
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
#include "WizardWidgetPage.h"
#include <SDL.h>
//---------------------------------------------------------------------------
class WizardWidgetButton : public WizardWidgetBase
{
protected:	
	/**
	 *	Describe drawing model, if button expands at the text size or doesn't
	 */
	bool Expands;
	/**
	 *	Say if the button is focused or it doesn't	
	 */
	bool Focused;

	/**
	 *	The containing text of the button	
	 */
	std::string Caption;

	void SetSurface(std::string FileName, int IndexSurface);

	/**
	 *	Paint expanded button, it gets the text size
	 */
	void WizardWidgetButton::PaintExpandedButton(
		SDL_Surface* BtnTexture, 
		SDL_Surface *BtnLeft, 
		SDL_Surface *BtnRight);
	/**
	 *	Paint unexpanded button, it gets the width size as reference
	 */
	void PaintNonExpandedButton(
		SDL_Surface* BtnTexture, 
		SDL_Surface *BtnLeft, 
		SDL_Surface *BtnRight);

protected:
	WizardWidgetPage* GetContainerPage();
public:
	WizardWidgetButton(SDLFrontEnd* FrontEnd, std::string Name);
	~WizardWidgetButton();

	/**
	 *	Paints the control and the children of the curent component
	 */
	virtual void Paint();
	/**
	 *	Set up the known attributes
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);
	/**
	 *	Say if button has focus or doesn't
	 */
	virtual void SetFocus(bool IsFocused);

	virtual std::string GetCaption();

	virtual void SetCaption(std::string Caption);

};

#endif
