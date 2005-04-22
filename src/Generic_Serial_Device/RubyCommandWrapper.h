/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef DCERUBYCOMMANDWRAPPER_H
#define DCERUBYCOMMANDWRAPPER_H

#include <map>
#include <string>

namespace DCE {

/**
@author Igor Spac,,,
*/
class RubyCommandWrapper {
public:
    RubyCommandWrapper(long devidfrom, long devidto, long priority, long type, long id);
    ~RubyCommandWrapper();

	void setParam(long id, const char* val) {
		params_[id] = val;
	}
	
	const char* getParam(long id) {
		return params_[id].c_str();
	}

	const std::map<long, std::string>& getParams() {
		return params_;
	}
	
	long getDevIdFrom() {
		return devidfrom_;
	}
	
	long getDevIdTo() {
		return devidto_;
	}
	
	long getPriority() {
		return priority_;
	}
	
	long getType() {
		return type_;
	}
	
	long getId() {
		return id_;
	}
	
public:
	long devidfrom_;
	long devidto_;
	long priority_;
	long type_;
	long id_;

	std::map<long, std::string> params_;
};

};

#endif
