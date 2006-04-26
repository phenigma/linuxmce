#include "SettingsDictionary.h"

SettingsDictionary::SettingsDictionary(void)
{
}

SettingsDictionary::~SettingsDictionary(void)
{
	this->Options.clear();
}

void SettingsDictionary::Set(std::string OptionName, std::string Value)
{
	Options[OptionName] = Value;
}

void SettingsDictionary::Set(std::string OptionName, int Value)
{
	//One int fits in 11 characters
	char Buffer[12];
	sprintf(Buffer, "%d", Value);
	Set(OptionName, Buffer);
}

bool SettingsDictionary::Exists(std::string OptionName)
{
	return (Options.find(OptionName) != Options.end());
}

std::string SettingsDictionary::GetValue(std::string OptionName)
{
	if (!Exists(OptionName))
		return "";
	return Options[OptionName];
}

void SettingsDictionary::SetName(std::string Name)
{
	this->Name = Name;
}

void SettingsDictionary::SetType(std::string Type)
{
	this->Type = Type;
}

std::string SettingsDictionary::GetName()
{
	return this->Name;
}

std::string SettingsDictionary::GetType()
{
	return this->Type;
}

void SettingsDictionary::Serialize(xmlTextWriterPtr Writer)
{
	int Result;
	std::string Text;

	Text = GetType();
	Result = xmlTextWriterWriteFormatElement(Writer, BAD_CAST "Type",
		"%s", Text.c_str());

	Text = GetName();
	Result = xmlTextWriterWriteFormatElement(Writer, BAD_CAST "Name",
		"%s", Text.c_str());

	/* Start an element named "Object". Since thist is the first
	* element, this will be the root element of the document. */
	Result = xmlTextWriterStartElement(Writer, BAD_CAST "Attributes");
	if (Result < 0) {
		printf
			("testXmlwriterFilename: Error at xmlTextWriterStartElement\n");
		return;
	}

	std::map <std::string, std::string>::iterator Item;
	for(Item = Options.begin(); Item != Options.end(); ++Item)
	{
		std::string AttributeName = Item->first;
		std::string Value = Item->second;
		Result = xmlTextWriterWriteFormatElement(Writer, BAD_CAST (AttributeName.c_str()),
			"%s", Value.c_str());
	}

	xmlTextWriterEndElement(Writer);

}

bool SettingsDictionary::HasAttributes()
{
	return Options.size() != 0;
}
