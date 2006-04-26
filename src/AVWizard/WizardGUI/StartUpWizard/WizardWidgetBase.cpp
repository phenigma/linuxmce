//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"
//---------------------------------------------------------------------------
#include "GUIWizardUtils.h"
//---------------------------------------------------------------------------

WizardWidgetBase::WizardWidgetBase(SDLFrontEnd* FrontEnd, std::string Name)
	: Parent(NULL)
{
	EventManager.reset(new WizardWidgetEventManager(this));
    this->Name = Name;
	this->FrontEnd = FrontEnd;
}

WizardWidgetBase::~WizardWidgetBase()
{
    std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); )
	{
        delete* Item;
        Item = Children.erase(Item);
    }
}

void WizardWidgetBase::SetParent(WizardWidgetBase* Parent)
{
	this->Parent = Parent;
	if(Parent == NULL)
		return;
	Parent->AddChild(this);
}

void WizardWidgetBase::DeleteChild(WizardWidgetBase* Child)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
	{
		if (*Item == Child) {
			Children.erase(Item);
			return;
		}
    }
}   

void WizardWidgetBase::AddChild(WizardWidgetBase* Child)
{
	if(Child == NULL)
		return;
	
	Child->Parent = this;
	Children.push_back(Child);
}

void WizardWidgetBase::SetUpAttribute(std::string Attribute, std::string DataValue)
{
	if(Attribute == "Width")
	{
		this->Width = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Height")
	{
		this->Height = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Left")
	{
		this->Left = Utils::StringToInt32(DataValue);
	}
	else
	if(Attribute == "Top")
	{
		this->Top = Utils::StringToInt32(DataValue);
	}

    Attributes[Attribute] = DataValue;
}

/*virtual*/ void WizardWidgetBase::ApplyAttributes()
{
    std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
        (*Item)->ApplyAttributes();

    Attributes.clear();
}

/*virtual*/ void WizardWidgetBase::Paint()
{
    std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
        (*Item)->Paint();
}

/*virtual*/ bool WizardWidgetBase::ExistAttribute(std::string Name)
{
    return Attributes.find(Name) != Attributes.end();
}

/*virtual*/ bool WizardWidgetBase::ApplyDictionary(SettingsDictionary* DefinitionDictionary)
{
	if(DefinitionDictionary == NULL)
		return false;

	std::map<std::string, std::string>::iterator AttributeItem;
	for(AttributeItem = DefinitionDictionary->Options.begin(); 
			AttributeItem != DefinitionDictionary->Options.end();
			AttributeItem++)
	{
		std::string AttributeName = AttributeItem->first;
		std::string AttributeValue = AttributeItem->second; 
		this->SetUpAttribute(AttributeName, AttributeValue);
	}

	ApplyAttributes();
	return true;
}

/*virtual*/ WizardWidgetBase* WizardWidgetBase::GetChild(std::string ChildName)
{
	std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
	{
		if ((*Item)->GetName() == ChildName) 
			return *Item;
	}

	return NULL;
}

/*virtual*/ WizardWidgetBase* WizardWidgetBase::GetChildRecursive(std::string ChildName)
{
	WizardWidgetBase* Result = GetChild(ChildName);
	if (Result != NULL)
		return Result;

	std::vector<WizardWidgetBase*>::iterator Item;
	for(Item = Children.begin(); Item<Children.end(); ++Item)
	{
		Result = (*Item)->GetChildRecursive(ChildName);
		if(Result != NULL)
			return Result;
	}
	return NULL;
}

std::string WizardWidgetBase::GetName()
{
	return Name;
}
