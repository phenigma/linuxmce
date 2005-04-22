/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/
#ifndef EMBRUBYFILECODESUPPLIER_H
#define EMBRUBYFILECODESUPPLIER_H

#include "RubyEmbeder.h"

#include <string>

namespace EMBRUBY {

/**
@author Igor Spac,,,
*/

class FileCodeSupplier : public EMBRUBY::RubyEmbederCodeSupplier {
public:
    FileCodeSupplier();
    virtual ~FileCodeSupplier();

public:
	void loadFile(const char* path) throw(RubyException);

protected:
	virtual const char* getRubyCode() {
		return code_.c_str();
	}

private:
	std::string code_;
};

};

#endif
