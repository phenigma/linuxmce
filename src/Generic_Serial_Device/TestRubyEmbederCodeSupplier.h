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
#ifndef EMBRUBYTESTRUBYEMBEDERCODESUPPLIER_H
#define EMBRUBYTESTRUBYEMBEDERCODESUPPLIER_H

#include "RubyEmbeder.h"

namespace EMBRUBY {

/**
@author Igor Spac,,,
*/
class TestRubyEmbederCodeSupplier : public RubyEmbederCodeSupplier {
public:
	const char* getRubyCode();
};

};

#endif
