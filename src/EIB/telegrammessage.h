//
// C++ Interface: telegrammessage
//
// Description: 
//
//
// Author:  <>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef EIBBUSTELEGRAMMESSAGE_H
#define EIBBUSTELEGRAMMESSAGE_H

#include "message.h"

#include <string>

#define MAX_STRING_DATA_LEN		23

namespace EIBBUS {

/**
@author 
*/
class TelegramMessage : public PeiMessage
{
	friend class MessagePool;
public:
    TelegramMessage();
	TelegramMessage(const TelegramMessage& tlmsg);
    ~TelegramMessage();

public:
	virtual int Send(BusConnector *pbusconn);
	virtual int Recv(BusConnector *pbusconn);
		
public:
	enum ACTIONTYPE {
		READ,
		ANSWER,
		WRITE/*,
		SUBSCRIBE,
		UNSUBSCRIBE,
		UNKNOWN*/
	};

public:
	inline void setActionType(ACTIONTYPE acttype) {
		acttype_ = acttype;
	}
	inline ACTIONTYPE getActionType() const {
		return acttype_;
	};
		
	inline void setGroupAddress(const char* gaddr) {
		gaddr_ = gaddr;
	}
	inline const char* getGroupAddress() const {
		return gaddr_.c_str();
	};
	
	
	inline unsigned int getDataLength() {
		return length_;
	}
	
	inline void setShortUserData(unsigned char usrdata) {
		shortusrdata_ = usrdata;
		length_ = 0;
	}
	inline unsigned char getShortUserData() const {
		return shortusrdata_;
	}
		
	inline void setUserData(const unsigned char* usrdata, unsigned int length) { /*in string format, which is then converted to apropriate format*/
		memcpy(usrdata_, usrdata, length);
		length_ = length;
	}
	inline const unsigned char* getUserData() const {
		return usrdata_;
	};

private:
	ACTIONTYPE acttype_;
	std::string gaddr_;
	
	unsigned int length_;
	unsigned char shortusrdata_;
	unsigned char usrdata_[MAX_STRING_DATA_LEN];
};

};

#endif
