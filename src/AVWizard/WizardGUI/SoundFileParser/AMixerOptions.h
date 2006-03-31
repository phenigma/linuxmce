#ifndef AMIXER_OPTIONS_H
#define AMIXER_OPTIONS_H

#include <vector>

class AMixerOptions
{
	std::vector<char*> Options;
public:
	virtual ~AMixerOptions(void);

	int AddOption(char* OptionName);

	int Locate(char* OptionName);
};

#endif
