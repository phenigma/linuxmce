/***************************************************************************
 *   Copyright (C) 2005 by Igor Spac,,,                                    *
 *   igor@dexx                                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "RubyEmbededClass.h"

#include <string>
#include <iostream>

using namespace std;

namespace EMBRUBY {

RubyEmbededClass::RubyEmbededClass() 
: value_(0) {}

RubyEmbededClass::~RubyEmbededClass()
{}

VALUE 
RubyEmbededClass::_callclass(VALUE arg) {
	ARGUMENTS& a = *reinterpret_cast<ARGUMENTS*>(arg);
	VALUE vclass = rb_const_get(rb_cObject, rb_intern(a.class_.classname_));
	return rb_funcall2(vclass, rb_intern(a.member_), a.argc_, a.argv_);
}

VALUE 
RubyEmbededClass::_callmethod(VALUE arg) {
	ARGUMENTS& a = *reinterpret_cast<ARGUMENTS*>(arg);
	cout << "Calling method " << a.member_ << " in class " << a.class_.classvalue_ << " with " << a.argc_ << " parameters" << endl;
	VALUE v = rb_funcall2(a.class_.classvalue_, rb_intern(a.member_), a.argc_, a.argv_);
	cout << "Method call ended." << endl;
	return v;
}

std::string 
RubyEmbededClass::_backtrace() {
	string bcktr;
	
    
	// position
	char tmpbuff[128];
	sprintf(tmpbuff, "\n\tin: %s: %d", ruby_sourcefile, ruby_sourceline);
    bcktr += tmpbuff;
	
    ID id = rb_frame_last_func();
    if(id) {
        bcktr += " :in `";
		bcktr += rb_id2name(id);
		bcktr += "'";
    }
	
	bcktr += "\n";
	
    // backtrace
    if(!NIL_P(ruby_errinfo)) {
        VALUE ary = rb_funcall(ruby_errinfo, rb_intern("backtrace"), 0);
        int c;
        for (c=0; c<RARRAY(ary)->len; c++) {
            bcktr += "\tfrom "; bcktr += RSTRING(RARRAY(ary)->ptr[c])->ptr; bcktr += "\n";
        }
    }
    return bcktr;
}

VALUE 
RubyEmbededClass::StrToValue(const char* str) {
	return rb_str_new2(str);
}

VALUE 
RubyEmbededClass::IntToValue(int n) {
	return INT2NUM(n);
}

};
