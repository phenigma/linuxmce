/**
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
//---------------------------------------------------------------------------
#ifndef WizardWidgetLabelH
#define WizardWidgetLabelH
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
//---------------------------------------------------------------------------
#include <string>
//---------------------------------------------------------------------------
enum FontAlign {
	AlignLeft = 0,
	AlignCenter = 1,
	AlignRight = 2
};
//---------------------------------------------------------------------------

class WizardWidgetLabel : public WizardWidgetBase
{
	std::string Caption;
	int Font_Height;
	int Align, Style;

	std::string Font_Name;
	TColorDesc* Font_Color;
public:
	WizardWidgetLabel(SDLFrontEnd* FrontEnd, std::string Name);
	virtual ~WizardWidgetLabel();

	virtual void SetUpAttribute(std::string Attribute, std::string DataValue);

	virtual void SetCaption(std::string Caption);

	virtual void Paint();
};
//---------------------------------------------------------------------------
#endif
