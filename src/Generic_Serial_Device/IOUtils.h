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
#ifndef DCEIOUTILS_H
#define DCEIOUTILS_H

#include <string>

namespace DCE {

/**
@author Igor Spac,,,
*/
class IOUtils{
public:
	static std::string FormatHexBuffer(const char* buff, unsigned int size);
};

};

#endif
