/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

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
