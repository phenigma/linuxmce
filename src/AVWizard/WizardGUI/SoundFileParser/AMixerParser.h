#ifndef AMIXER_PARSER_H
#define AMIXER_PARSER_H

#include "AMixerOptions.h"

class AMixerParser
{
	void AMixerParser::GetControlLowerAndUpper(char* Line, int& LowerBound, int& UpperBound);
	bool GetControlLimits(char* Line, int& LowerBound, int& UpperBound, int& Value);

	void GetControlValue(char* Line, int& Value);

public:
	AMixerParser();
	~AMixerParser();

	AMixerOptions* ParseConfig();

	void BuildOptions(AMixerOptions* Options, char* Line);

	bool GetControlOptions(char* ControlName, int& LowerBound, int& UpperBound, int& Value);
};

#endif