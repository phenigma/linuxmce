/***************************************************************************
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
#ifndef EIBMESSAGEPOOL_H
#define EIBMESSAGEPOOL_H

#include "thread.h"
#include "mutex.h"
#include "event.h"
#include "statemachine.h"
#include "telegrammessage.h"

#include <list>

namespace EIBBUS {

class BusConnector;

/**
@author igor
*/

class MessagePoolInterceptor {
public:
	virtual void handleTelegram(const TelegramMessage *pt) = 0;
};

class MessagePool : public Thread, protected StateMachine {
public:
    MessagePool();
    ~MessagePool();
	
public:
	int regInterceptor(MessagePoolInterceptor *pi);
	void unregInterceptor(MessagePoolInterceptor *pi);

public:
	int sendTelegram(const TelegramMessage *ptelegram);
	int readTelegram(TelegramMessage *ptelegram, int waitmilisecs = -1);

protected:
	virtual void* _Run();
	
	virtual bool handleStartup();
	virtual void handleTerminate();

private:
	void handleState();
	void logTelegram(const TelegramMessage *pt);
	
private:
	BusConnector* pbusconn_;

private:
	MessagePoolInterceptor* pi_;

private:
	Mutex msq_; /*lock on both queues*/
	Event esq_, erq_; /*event for arriving in send/recv queues*/
	std::list<TelegramMessage> sendqueue_;
	std::list<TelegramMessage> recvqueue_;
};

};

#endif
