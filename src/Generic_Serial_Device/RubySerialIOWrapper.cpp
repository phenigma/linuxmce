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
#include "RubySerialIOWrapper.h"

namespace DCE {

RubySerialIOWrapper::RubySerialIOWrapper()
	: pdce_(NULL)
{}

RubySerialIOWrapper::~RubySerialIOWrapper()
{}

void 
RubySerialIOWrapper::SendCommand(RubyCommandWrapper* pcmd) {
	if(pdce_) {
		pdce_->SendCommand(pcmd);
	}
}

};
