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

namespace EIBBUS {

/**
@author 
*/
class TelegramMessage : public ServerMessage
{
	friend class MessagePool;
public:
    TelegramMessage();
    ~TelegramMessage();

public:
	enum ACTIONTYPE {
		READ,
		ANSWER,
		WRITE,
		SUBSCRIBE,
		UNSUBSCRIBE,
		UNKNOWN
	};
	
	enum DATATYPE {
		DT_SWITCH 				= 1, 		/*1 bit*/
		DT_DIMMING_CONTROL 		= 2,		/*4 bit*/
		DT_TIME			 		= 3,		/*3 Bytes*/
		DT_DATE			 		= 4,		/*3 Bytes*/
		DT_VALUE		 		= 5,		/*2 Bytes*/
		DT_SCALING		 		= 6,		/*1 Byte*/
		DT_DRIVE_CONTROL		= 7,		/*1 bit*/
		DT_FLOAT				= 9,		/*4 Bytes*/
		DT_COUNTER16			= 10,		/*2 Bytes*/
		DT_COUNTER32			= 11,		/*4 Bytes*/
		DT_ASCIICHAR			= 13,		/*1 Byte*/
		DT_COUNTER8				= 14,		/*1 Byte*/
		DT_STRING				= 15		/*23 Bytes*/
	};

public:
	
	inline void setEisType(int eistype) {
		eistype_ = eistype;
	}
	inline int getEisType() const {
		return eistype_;
	}
	
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
	
	inline void setUserData(const char* usrdata) {
		usrdata_ = usrdata;
	}
	inline const char* getUserData() const {
		return usrdata_.c_str();
	};
	

private:
	int eistype_;
	ACTIONTYPE acttype_;
	std::string gaddr_;
	std::string usrdata_;
	
protected:
	virtual int Send(BusConnector *pbusconn);
	virtual int Recv(BusConnector *pbusconn);
};

};

#endif
