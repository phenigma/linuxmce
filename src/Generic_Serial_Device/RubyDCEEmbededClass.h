/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

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
    RubyDCEEmbededClass(RubyDCECodeSupplier* pcs, int dwPK_Device);
    ~RubyDCEEmbededClass();

public:
	void CallCmdHandler(Message *pMessage);
	void CallCmdForChildHandler(Message *pMessage);

private:
	RubyDCECodeSupplier* pcs_;
};

};

#endif
