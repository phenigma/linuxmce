#include "AVWizardConfParser.h"

AVWizardConfParser::AVWizardConfParser(void)
{
#ifdef WIN32
	DefaultConfigFileName = "avwizardsettings.xml";
#else
	DefaultConfigFileName = "avwizardsettings.xml";
#endif	
}

AVWizardConfParser::~AVWizardConfParser(void)
{
}

SettingsDictionaryTree* AVWizardConfParser::ParseFile(std::string FileName)
{
	SettingsDictionaryTree* Result = new SettingsDictionaryTree();
	if (FileName == "")
		FileName = DefaultConfigFileName;
	Result->LoadFromXMLFile(FileName);
	return Result;
}
