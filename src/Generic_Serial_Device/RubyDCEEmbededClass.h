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
#ifndef DCERUBYDCEEMBEDEDCLASS_H
#define DCERUBYDCEEMBEDEDCLASS_H

#include "RubyEmbededClass.h"
#include "RubyDCECodeSupplier.h"

namespace DCE {

class Message;
class RubyDeviceWrapper;

/**
@author Igor Spac,,,
*/
class RubyDCEEmbededClass : public EMBRUBY::RubyEmbededClassImpl<RubyDCEEmbededClass> {
public:
	DEFINE_CLASS_NAME("DCE");
	
	RubyDCEEmbededClass();
    RubyDCEEmbededClass(int dwPK_Device);
    ~RubyDCEEmbededClass();

public:
	void CallCmdHandler(RubyDCECodeSupplier* pcg, Message *pMessage);
};

};

#endif
