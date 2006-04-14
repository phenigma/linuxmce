#include "SoundUtils.h"

#include "AMixerParser.h"

AMixerOptions* GetCurentMixerOptions()
{
	AMixerParser* Parser = new AMixerParser();

	AMixerOptions* Result = Parser->ParseConfig();

	delete Parser;

	return Result;
}

bool GetControlLimits(char* ControlName, int& LowerBound, int& UpperBound, int& Value)
{
	if(ControlName == NULL)
		return false;

	if(strlen(ControlName) == 0)
		return false;

	AMixerParser* Parser = new AMixerParser();

	bool Result = Parser->GetControlOptions(ControlName, LowerBound, UpperBound, Value);

	delete Parser;

	return Result;
}

void SetControlLimits(char* ControlName, int Value)
{
#ifdef WIN32
	char Command[1024];
	sprintf(Command, "amixer sset Master Playback %s", Value);
	system(Command);
#endif
}
