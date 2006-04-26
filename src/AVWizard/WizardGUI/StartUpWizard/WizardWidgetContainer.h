/**
*	Created by CipLogic < ciprian dot m at plutohome dot com >
*/
//---------------------------------------------------------------------------
#ifndef WizardWidgetContainer_H_
#define WizardWidgetContainer_H_
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
//---------------------------------------------------------------------------
class WizardWidgetContainer: public WizardWidgetBase
{
	bool Visible;
public:
	WizardWidgetContainer(SDLFrontEnd* FrontEnd, std::string Name);
	~WizardWidgetContainer();

	virtual void Paint();
	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);
};
#endif
