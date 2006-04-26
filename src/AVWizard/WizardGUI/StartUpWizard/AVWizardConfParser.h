#ifndef AVWizardConfParser_H_
#define AVWizardConfParser_H_

#include "SettingsDictionaryTree.h"

class AVWizardConfParser
{
	std::string DefaultConfigFileName;
public:
	AVWizardConfParser(void);
	virtual ~AVWizardConfParser(void);

	SettingsDictionaryTree* ParseFile(std::string FileName = "");

};

#endif
