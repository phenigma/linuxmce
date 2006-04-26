#include "AMixerOptions.h"

AMixerOptions* AMixerOptions::Instance = NULL;

AMixerOptions* AMixerOptions::GetInstance()
{
	if(Instance == NULL)
		Instance = new AMixerOptions();
	
	return Instance;
}

void AMixerOptions::CleanUp()
{
	if(Instance != NULL)
	{
		delete Instance;
		Instance = NULL;		
	}
}

AMixerOptions::AMixerOptions()
{
}

AMixerOptions::~AMixerOptions()
{
}


void AMixerOptions::AddOption(char* OptionName)
{
	Options.Set(OptionName, 0);
}

bool AMixerOptions::Exists(char* OptionName)
{
	return Options.Exists(OptionName);
}
