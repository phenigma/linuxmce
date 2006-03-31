#include "settingsdictionary.h"

SettingsDictionary::SettingsDictionary(void)
{
	Options["Title"] = 0;
	Options["Width"] = 0;
	Options["Height"] = 0;
	Options["Refresh"] = 0;
	Options["LowerBound"] = 0;
	Options["UpperBound"] = 0;
	Options["Value"] = 0;
}

SettingsDictionary::~SettingsDictionary(void)
{
	this->Options.clear();
}

void SettingsDictionary::Set(std::string OptionName, int Value)
{
	Options[OptionName] = Value;
}

bool SettingsDictionary::Exists(std::string OptionName)
{
	return (Options.find(OptionName) != Options.end());
}

int SettingsDictionary::GetValue(std::string OptionName)
{
	if (!Exists(OptionName))
		return -1;
	return Options[OptionName];
}
