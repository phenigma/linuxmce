//
// C++ Implementation: socket
//
// Description: 
//
//
// Author:  <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "token.h"

using namespace std;

namespace ASTERISK {

Token::Token() {
}


Token::~Token()
{
}

void Token::setKey(std::string key, std::string value) {
	tokmap[key] = value;
}

string& Token::getKey(std::string key) {
	return tokmap[key];
}

bool Token::hasKey(std::string key) {
	return tokmap.find(key) != tokmap.end();
}
	    
std::string Token::serialize() const {
	string ret;
	
	map<string, string>::const_iterator it = tokmap.begin();
	while(it != tokmap.end()) {
		ret += (*it).first;
		ret += ": ";
		ret += (*it).second;
		ret += "\x0D\x0A";
			
		it++;
	}
	ret += "\x0D\x0A";
	

	return ret;
}

void Token::removeAll() {
	tokmap.clear();
}

int Token::unserialize(std::string str) {
	removeAll();
	
	string key, value;
	
	int keyend, valueend = 0;
	while(1) {
		keyend = str.find(": ", valueend);
		if(keyend != -1) {
			keyend += 2;
			key = str.substr(valueend, keyend - valueend -2 /*dd*/);
			valueend = str.find("\r\n", keyend);
			if(valueend != -1) {
				valueend += 2;
				value = str.substr(keyend, valueend - keyend  - 2/*eol*/);

				setKey(key, value);
			} else {
				break;
			}
		} else {
			break;
		}
	}
			
	return 0;	
}

}
