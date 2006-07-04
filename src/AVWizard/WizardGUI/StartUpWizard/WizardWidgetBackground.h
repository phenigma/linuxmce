/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//---------------------------------------------------------------------------
#ifndef WizardWidgetBackground_H
#define WizardWidgetBackground_H
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
#include "SDL.h"
//---------------------------------------------------------------------------
class WizardWidgetBackground : public WizardWidgetBase
{
	TColorDesc* Color;
	SDL_Surface* Surface;
	/**
	 *	Set up the surface from an existing file
	 */
	virtual void SetSurface(std::string FileName);
public:
	/**
	 *	Default constructor
	 */
	WizardWidgetBackground(SDLFrontEnd* FrontEnd, std::string Name);
	/**
	 *	Default destructor
	 */
	virtual ~WizardWidgetBackground();
	/**
	 *	Paints the control and the children of the curent component
	 */
	virtual void Paint();

	/**
	 *	Set up the known attributes
	 */
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);

};
//---------------------------------------------------------------------------
#endif
