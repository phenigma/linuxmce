//
// C++ Implementation: message
//
// Description: 
//
//
// Author: igor <igor@dexx>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "message.h"

namespace CM11ADEV {

unsigned char Message::s_MapHouseDeviceCodes[] =  {
	0x06, //A
	0x0E, //B
	0x02, //C
	0x0A, //D
	0x01, //E
	0x09, //F
	0x05, //G
	0x0D, //H
	0x07, //I
	0x0F, //J
	0x03, //K
	0x0B, //L
	0x00, //M
	0x08, //N
	0x04, //O
	0x0C  //P
};

Message::Message() 
	: houseCode_(0), deviceCode_(0), functionCode_(0), dimmLevel_(0)
{
}

Message::~Message() {
}


void 
Message::setAddress(std::string address) {// adress in the form of A1, B2,...
	int len = address.length();
	if(len < 2 || len > 3) {
		return;
	}
	
	if(address[0] >= 'A' && address[0] <= 'P') {
		houseCode_ = s_MapHouseDeviceCodes[address[0] - 'A'];
	}
	
	int devcode = atoi(address.substr(1, len - 1).c_str());
	if(devcode >= 1 && devcode <= 16) {
		deviceCode_ = s_MapHouseDeviceCodes[devcode - 1];
	}
}

};
