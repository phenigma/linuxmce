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
#include "IOUtils.h"

namespace DCE {

std::string 
IOUtils::FormatHexBuffer(const char* buff, unsigned int size) {
	std::string logstr;
	if(size <= 0) {
		logstr = "EMPTY BUFFER";
	} else {
		char hxbuff[5];
		for(unsigned int i = 0; i < size; i++) {
			sprintf(hxbuff, "0x%0hhx", buff[i]);
			logstr += ((i > 0) ? " " : "");
			logstr += hxbuff;
		}
	}
	return logstr;
}

};
