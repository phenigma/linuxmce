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
	
public:
	const char* getRubyCode() {
		return rcode_.c_str();
	}

private:
	std::string rcode_;
};

};

#endif
