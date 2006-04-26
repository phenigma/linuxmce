#include "WizardWidgetContainer.h"
#include "GUIWizardUtils.h"

WizardWidgetContainer::WizardWidgetContainer(SDLFrontEnd* FrontEnd, std::string Name)
: WizardWidgetBase(FrontEnd, Name)
{
	this->Visible = true;
}

WizardWidgetContainer::~WizardWidgetContainer()
{

}

/*virtual*/ void WizardWidgetContainer::Paint()
{
	if(!Visible)
		return;
	WizardWidgetBase::Paint();
}

/*virtual*/ void WizardWidgetContainer::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Visible")
		DataValue = (Utils::StringToInt32(DataValue)!= 0);
	else
		WizardWidgetBase::SetUpAttribute(Attribute, DataValue);
}
