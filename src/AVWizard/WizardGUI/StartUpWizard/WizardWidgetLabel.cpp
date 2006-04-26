//---------------------------------------------------------------------------
#include "WizardWidgetLabel.h"
//---------------------------------------------------------------------------
#include "GUIWizardUtils.h"
#include <SDL_ttf.h>
//---------------------------------------------------------------------------

WizardWidgetLabel::WizardWidgetLabel(SDLFrontEnd* FrontEnd, std::string Name)
	: WizardWidgetBase(FrontEnd, Name)
{
	Font_Color = NULL;
	Align = AlignLeft;
	Style = 0;
}

WizardWidgetLabel::~WizardWidgetLabel()
{

}

/*virtual*/ void WizardWidgetLabel::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "FontHeight")
	{
		Font_Height = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Caption")
	{
		Caption = DataValue;
	}
	else
	if(Attribute == "FontName")
	{
		Font_Name = DataValue;
	}
	else
	if(Attribute == "Align")
	{
		if (DataValue == "Left")
		{
			Align = AlignLeft;
		}
		else
		if (DataValue == "Center")
		{
			Align = AlignCenter;
		}
		else
		if (DataValue == "Right")
		{
			Align = AlignRight;
		}
		else
			Align = AlignLeft;
	}
	else
	if(Attribute == "Style")
	{
		if (DataValue == "Regular")
		{
			Style = 0;
		}
		else
		if (DataValue == "Bold")
		{
			Style = TTF_STYLE_BOLD;
		}
		else
		if (DataValue == "Italic")
		{
			Style = TTF_STYLE_ITALIC;
		}
		else
		if (DataValue == "Bold-Italic")
		{
			Style = TTF_STYLE_ITALIC | TTF_STYLE_BOLD;
		}
		else
			Style = 0;
	}
	else
	if(Attribute == "FontColor")
	{
		Font_Color = Utils::StringToColorDesc(DataValue);
	}
	else
        WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}

/*virtual*/ void WizardWidgetLabel::Paint()
{
	FrontEnd->SetCurrentFont(Font_Name, Font_Height, Style);
	FrontEnd->TextOutput((char*)Caption.c_str(), Left, Top, *Font_Color, Align);
	WizardWidgetBase::Paint();
}

/*virtual*/ void WizardWidgetLabel::SetCaption(std::string Caption)
{
	this->Caption = Caption;
}
