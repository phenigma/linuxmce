#ifndef AMIXER_PARSER_H
#define AMIXER_PARSER_H

#include "AMixerOptions.h"

/**
 *  Singleton class parse the Linux's command 'amixer' output
 *	Created by CipLogic < ciprian dot m at plutohome dot com >
 */
class AMixerParser
{
	void AMixerParser::GetControlLowerAndUpper(char* Line, int& LowerBound, int& UpperBound);
	bool GetControlLimits(char* Line, int& LowerBound, int& UpperBound, int& Value);

	void GetControlValue(char* Line, int& Value);

	/**
	 *	Singleton class - default constructor is 	
	 */
	AMixerParser();
	static AMixerParser* Instance;

public:
	/**
	 *	Singleton method for get the singleton's instance 
	 */
	static AMixerParser* GetInstance();
	/**
	 *	Singleton method to force cleanup the code
	 */
	void CleanUp();

	/**
	 *	(Virtual) Default destructor
	 */
	virtual ~AMixerParser();

	/**
	 *	
	 */
	AMixerOptions* ParseConfig(std::string FileName);

	void BuildOptions(AMixerOptions* Options, char* Line);

	bool GetControlOptions(char* ControlName, int& LowerBound, int& UpperBound, int& Value);
};

#endif
