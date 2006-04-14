#include "amixeroptions.h"

AMixerOptions::~AMixerOptions(void)
{
	std::vector <char*>::iterator Item = Options.begin();
	while(Item < Options.end())
	{
		free(*Item);
		Item++;
	}

}

int AMixerOptions::AddOption(char* OptionName)
{
	size_t LenString = strlen(OptionName);
	char* CopyString = (char*) malloc (LenString+1);
	strncpy(CopyString, OptionName, LenString+1);
	Options.push_back(CopyString);

	return -1;
}

int AMixerOptions::Locate(char* OptionName)
{
	std::vector <char*>::iterator Item = Options.begin();
	int Position = 0;
	while(Item < Options.end())
	{
		if (strcmp(OptionName, *Item) == 0)
			return Position;

		Position++;
		Item++;
	}
	return -1;
}