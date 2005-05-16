/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef DCERUBYDCECODESUPPLIER_H
#define DCERUBYDCECODESUPPLIER_H

#include "RubyEmbeder.h"

#include <map>
#include <list>

class Database_pluto_main;

namespace DCE {

class DeviceData_Impl;
class Command_Impl;

/**
@author Igor Spac,,,
*/
class RubyDCECodeSupplier : public EMBRUBY::RubyEmbederCodeSupplier {
public:
    RubyDCECodeSupplier();
    virtual ~RubyDCECodeSupplier();

public:
	void addCode(Database_pluto_main* pdb, Command_Impl *pcmdimpl, DeviceData_Impl* pdevicedata);
	void clearCode();
	
public:
	const char* getRubyCode() {
		return rcode_.c_str();
	}

	bool isCmdImplemented(int cmd);
	bool isPrivateAssigned() {	
		return privateassigned_;
	}
	bool isProcChildCommandAssigned() {
		return procchildcmdassigned_;
	}
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
	bool privateassigned_;
	bool procchildcmdassigned_;

private:
	std::string rcode_;
};

};

#endif
