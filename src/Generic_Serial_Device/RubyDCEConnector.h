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
#ifndef DCERUBYDCECONNECTOR_H
#define DCERUBYDCECONNECTOR_H

#include "RubyCommandWrapper.h"

namespace DCE {

/**
@author Igor Spac,,,
*/
class RubyDCEConnector {
public:
	/*methods for comunicating with DCE */
	virtual void SendCommand(RubyCommandWrapper* pcmd) = 0;
};

};

#endif
