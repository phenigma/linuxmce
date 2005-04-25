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
#ifndef EIBBUSDUPLICATEFILTERPOOLINTERCEPTOR_H
#define EIBBUSDUPLICATEFILTERPOOLINTERCEPTOR_H

#include "messagepool.h"

#include <list>

namespace EIBBUS {

/**
@author 
*/
class DuplicateFilterPoolInterceptor : public MessagePoolInterceptor {
public:
    DuplicateFilterPoolInterceptor();
    virtual ~DuplicateFilterPoolInterceptor();

public:
	virtual void handleUniqueTelegram(const TelegramMessage *pt) = 0;
	
protected:
	virtual void handleTelegram(const TelegramMessage *pt);

private:
	typedef struct _TIMEDMESSAGE {
		TelegramMessage msg;
		struct timespec atm;
		_TIMEDMESSAGE(const TelegramMessage& msg, struct timespec& atm) {
			this->msg = msg;
			this->atm = atm;
		}
	} TIMEDMESSAGE;
		
	std::list<TIMEDMESSAGE> messages_;
};

};

#endif
