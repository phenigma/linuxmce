/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//---------------------------------------------------------------------------
#ifndef WizardWidgetImageH
#define WizardWidgetImageH
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
#include "SDL.h"
//---------------------------------------------------------------------------
class WizardWidgetImage : public WizardWidgetBase
{
	SDL_Surface* Surface;
	/**
	 *	Set up the surface from an existing file
	 */
	virtual void SetSurface(std::string FileName);
public:
	/**
	 *	Default constructor
	 */
	WizardWidgetImage(SDLFrontEnd* FrontEnd, std::string Name);
	/**
	 *	Default destructor
	 */
	virtual ~WizardWidgetImage();
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
