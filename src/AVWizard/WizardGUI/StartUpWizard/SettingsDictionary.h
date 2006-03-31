#pragma once

#include <map>
#include <string>

class SettingsDictionary
{
	std::map <std::string, int> Options;
public:
	SettingsDictionary();
	~SettingsDictionary();

	void Set(std::string OptionName, int Value);
	bool Exists(std::string OptionName);

	int GetValue(std::string OptionName);
};
