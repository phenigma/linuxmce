#include "WizardConfigParser.h"
//---------------------------------------------------------------------------
#include <stdlib.h>
//---------------------------------------------------------------------------
WizardConfigParser* WizardConfigParser::Instance = NULL;
//---------------------------------------------------------------------------
WizardConfigParser::WizardConfigParser(void)
{

}
//---------------------------------------------------------------------------
WizardConfigParser::~WizardConfigParser(void)
{

}
//---------------------------------------------------------------------------
WizardConfigParser* WizardConfigParser::GetInstance()
{
	if(Instance == NULL)
		Instance = new WizardConfigParser();
	return Instance;
}
//---------------------------------------------------------------------------
void WizardConfigParser::SetOption(std::string OptionName, 
												std::string OptionValue)
{

}

std::string WizardConfigParser::GetOption(std::string OptionName)
{
	return this->Options[OptionName];
}

void WizardConfigParser::CleanUpOptions()
{
	this->Options.clear();
}

void WizardConfigParser::Serialize(std::string XMLFile)
{

}

void WizardConfigParser::Deserialize(std::string XMLFile)
{

}
