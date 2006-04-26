#include "SoundUtils.h"

#include "AMixerParser.h"

AMixerOptions* GetCurentMixerOptions()
{
	AMixerParser* Parser = AMixerParser::GetInstance();

	AMixerOptions* Result = Parser->ParseConfig("");

	Parser->CleanUp();

	return Result;
}

bool GetControlLimits(char* ControlName, int& LowerBound, int& UpperBound, int& Value)
{
	if(ControlName == NULL)
		return false;

	if(strlen(ControlName) == 0)
		return false;

	AMixerParser* Parser = AMixerParser::GetInstance();

	bool Result = Parser->GetControlOptions(ControlName, LowerBound, UpperBound, Value);

	Parser->CleanUp();

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
