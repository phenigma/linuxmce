//---------------------------------------------------------------------------
#include "WizardWidgetsFactory.h"
//---------------------------------------------------------------------------
#include "WizardWidgetPage.h"
#include "WizardWidgetLabel.h"
#include "WizardWidgetImage.h"
#include "WizardWidgetListBox.h"
#include "WizardWidgetButton.h"
#include "WizardWidgetContainer.h"
//---------------------------------------------------------------------------
WizardWidgetsFactory* WizardWidgetsFactory::Instance = NULL;
//---------------------------------------------------------------------------
WizardWidgetsFactory::WizardWidgetsFactory()
{
	FrontEnd = NULL;
    WidgetTypes["Page"] = WIDGET_PAGE;
	WidgetTypes["Image"] = WIDGET_IMAGE;
	WidgetTypes["Label"] = WIDGET_LABEL;
	WidgetTypes["ListBox"] = WIDGET_LISTBOX;
	WidgetTypes["Button"] = WIDGET_BUTTON;
	WidgetTypes["Container"] = WIDGET_CONTAINER;
}
//---------------------------------------------------------------------------
WizardWidgetsFactory* WizardWidgetsFactory::GetInstance()
{
	if(!Instance)
		Instance = new WizardWidgetsFactory();

	return Instance;
}
//---------------------------------------------------------------------------
WizardWidgetBase* WizardWidgetsFactory::CreateWidget(int ID, std::string Name)
{
	switch(ID)
	{
		case WIDGET_PAGE:
			return new WizardWidgetPage(FrontEnd, Name);
		case WIDGET_IMAGE:
			return new WizardWidgetImage(FrontEnd, Name);
		case WIDGET_LABEL:
			return new WizardWidgetLabel(FrontEnd, Name);
		case WIDGET_LISTBOX:
        	return new WizardWidgetListBox(FrontEnd, Name);
		case WIDGET_BUTTON:
			return new WizardWidgetButton(FrontEnd, Name);
		case WIDGET_CONTAINER:
			return new WizardWidgetContainer(FrontEnd, Name);
	}
	return NULL;
}
//---------------------------------------------------------------------------
void WizardWidgetsFactory::CleanUp()
{
	delete Instance;
	Instance = NULL;
}
//---------------------------------------------------------------------------
int WizardWidgetsFactory::GetWidgetType(std::string TypeName)
{
	return WidgetTypes[TypeName];
}
//---------------------------------------------------------------------------
void WizardWidgetsFactory::SetSDLFrontEnd(SDLFrontEnd* FrontEnd)
{
	this->FrontEnd = FrontEnd;
}
//---------------------------------------------------------------------------
WizardWidgetBase* WizardWidgetsFactory::CreateWidget(SettingsDictionaryTree* 
	DialogSource)
{
	if(DialogSource == NULL)
		return NULL;
	
	SettingsDictionary* Dictionary = DialogSource->GetDictionary();
	int IDType = WidgetTypes[Dictionary->GetType()];
	std::string Name = Dictionary->GetName();

	WizardWidgetBase* Result = CreateWidget(IDType, Name);
	if(Result == NULL)
		return Result;
	Result->ApplyDictionary(Dictionary);

	std::vector<SettingsDictionaryTree*>::iterator Child;
	for(Child = DialogSource->Children.begin(); 
		Child != DialogSource->Children.end();
		++Child)
	{
		WizardWidgetBase* ControlChild = CreateWidget(*Child);

		Result->AddChild(ControlChild);
	}

	return Result;
}
//---------------------------------------------------------------------------
