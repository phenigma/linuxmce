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
#include "RubyCommandWrapper.h"

namespace DCE {

RubyCommandWrapper::RubyCommandWrapper(long devidfrom, long devidto, long priority, long type, long id)
	: devidfrom_(devidfrom), devidto_(devidto), priority_(priority), type_(type), id_(id)
{}


RubyCommandWrapper::~RubyCommandWrapper() 
{}


};
