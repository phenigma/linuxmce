#include <stdio.h>

#include "AMixerParser.h"

int main()
{
	AMixerParser * Parser = new AMixerParser();

	Parser->ParseConfig();

	int Lower, Upper, Value;

	Parser->GetControlOptions("Master", Lower, Upper, Value);

	delete Parser;
	return 0;
}