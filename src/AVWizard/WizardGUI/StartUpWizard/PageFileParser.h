//---------------------------------------------------------------------------

#ifndef PageFileParserH
#define PageFileParserH
//---------------------------------------------------------------------------
#include <fstream>
//---------------------------------------------------------------------------
#include "WizardWidgetBase.h"

/**
 *	Singleton class that offers generation pages from files capabilities
 */
class PageFileParser
{
	WizardWidgetBase* ParseObjectLine(char* LineToParse, WizardWidgetBase* Parent);
	
	void ParseLine(char* LineToParse, WizardWidgetBase* & Control);
	void ParseAttribute(std::ifstream &FInput, char* LineToParse, WizardWidgetBase* & Control);
	bool IgnoreCurrentLine(char* LineToParse);

	static PageFileParser* Instance;
	/**
	 *	void constructor, created to make that class singleton
	 */
	PageFileParser(){}
public:
	WizardWidgetBase* LoadFromFile(char* FileName);

	static PageFileParser* GetInstance();
	void CleanUp();
};
//---------------------------------------------------------------------------
#endif
