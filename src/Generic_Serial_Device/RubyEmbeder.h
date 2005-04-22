/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef RUBYEMBEDER_H
#define RUBYEMBEDER_H

#include "RubyExceptions.h"

#include <ruby.h>

namespace EMBRUBY {

/**
@author Igor Spac,,,
*/

class RubyEmbederCodeSupplier;

class RubyEmbeder {
public:
    RubyEmbeder();
    ~RubyEmbeder();

public:
	void loadCode(RubyEmbederCodeSupplier *psup) throw(RubyException);

public:
	static RubyEmbeder* getInstance();

private:
	static VALUE _loadcode(VALUE arg);
	
private:
	static RubyEmbeder* s_instance_;
};

class RubyEmbederCodeSupplier {
	friend class RubyEmbeder;
protected:
	virtual const char* getRubyCode() = 0;
};

}

#endif
