//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DCERUBYDCECODESUPPLIER_H
#define DCERUBYDCECODESUPPLIER_H

#include "RubyEmbeder.h"

#include <map>
#include <list>

class Database_pluto_main;

namespace DCE {

class DeviceData_Base;

/**
@author Igor Spac,,,
*/
class RubyDCECodeSupplier : public EMBRUBY::RubyEmbederCodeSupplier {
public:
    RubyDCECodeSupplier();
    virtual ~RubyDCECodeSupplier();

public:
	void addCode(Database_pluto_main* pdb, DeviceData_Base* pdevicedata);
	void clearCode();
	
public:
	const char* getRubyCode() {
		return rcode_.c_str();
	}

	bool isCmdImplemented(int cmd);
	int getParamsOrderForCmd(/*in*/int cmd, /*out*/std::list<int>& params);
	int getParamsNamesForCmd(/*in*/int cmd, /*out*/std::list<std::string>& params);
	
private:
	void FillClassMembersFromDevice(Database_pluto_main* pdb, long id, bool isDevTemplate);
	std::string TranslateCommandToRuby(const std::string& cmdtxt);

private:
	/*command to param names mapping*/
	typedef 
		std::pair<int, std::string> PARAMPAIR; /*list of pairs: param id and ruby param name*/
	typedef 
		std::list< std::pair<int, std::string> > PARAMLIST; /*list of pairs: param id and ruby param name*/
	typedef 
		std::map<int, PARAMLIST> COMMANDPARAMMAP; /*map between command id and param map*/

	COMMANDPARAMMAP cmdparammap_;

private:
	std::string rcode_;
};

};

#endif
