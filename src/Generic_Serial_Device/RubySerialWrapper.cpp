//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "RubySerialWrapper.h"

namespace DCE {

RubySerialWrapper::RubySerialWrapper()
	: pdce_(NULL)	
{
}

RubySerialWrapper::~RubySerialWrapper()
{
}

void 
RubySerialWrapper::SendCommand(RubyCommandWrapper* pcmd) {
	if(pdce_) {
		pdce_->SendCommand(pcmd);
	}
}

};
