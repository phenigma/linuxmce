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
#include "RubySerialIOConnectionWrapper.h"

namespace DCE {

RubySerialIOConnectionWrapper::RubySerialIOConnectionWrapper(IOConnection* pconn)
	: BufferedIOConnection(pconn)
{
}


RubySerialIOConnectionWrapper::~RubySerialIOConnectionWrapper()
{
}


};
