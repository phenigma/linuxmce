/***************************************************************************
 *   Copyright (C) 2005 by igor                                            *
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
#include "messagepool.h"
#include "busconnector.h"

#include "DCE/Logger.h"

#include "bauinitmessage.h"
#include "monitorrequestmessage.h"
#include "ackmessage.h"


using namespace std;
using namespace DCE;

#define SLEEP_IO_ERROR			5000
#define SLEEP_WAIT_MESSAGES		100

/* states */
#define STATE_READY				1
#define STATE_SLEEP				2
#define STATE_INITIALIZEBUS		3
#define STATE_INITIALIZEREAD	4

#define MAX_RECVQUEUE_SIZE		20

namespace EIBBUS {

MessagePool::MessagePool() {
	pbusconn_ = BusConnector::getInstance();
}


MessagePool::~MessagePool()
{
}

int 
MessagePool::regInterceptor(MessagePoolInterceptor *pi) {
	pi_ = pi;
	return 0;
}

void 
MessagePool::unregInterceptor(MessagePoolInterceptor *pi) {
	pi_ = NULL;
}

bool 
MessagePool::handleStartup()
{
	return true;
}

void 
MessagePool::handleTerminate()
{
}

void* 
MessagePool::_Run() {
	while(!isStopRequested()) {
		/*check if we are connected*/
		if(!pbusconn_->isOpened()) {
			if(pbusconn_->Open()) {
				g_pPlutoLogger->Write(LV_STATUS, "Could not connect to EIB Bus. Sleeping for %d second...", SLEEP_IO_ERROR);
				sleep(SLEEP_IO_ERROR / 1000);
				continue;
			}
			setState(STATE_INITIALIZEBUS);
		} else
		if(pbusconn_->isDataAvailable()) {
			setState(STATE_INITIALIZEREAD);
		}
		
		handleState();
	}	
	return 0;
}

void 
MessagePool::handleState() {
	switch(getState()) {
		case STATE_INITIALIZEBUS: {
			g_pPlutoLogger->Write(LV_STATUS, "Sending BAU Initialization.");
		
			BauInitMessage bimsg;
			if(bimsg.Send(pbusconn_)) {
				g_pPlutoLogger->Write(LV_WARNING, "Error sending BAU Initialization. Waiting for next retry.");
				return;
			}
			
			g_pPlutoLogger->Write(LV_STATUS, "Requesting Monitor mode.");

			MonitorRequestMessage mrmsg;
			if(mrmsg.Send(pbusconn_)) {
				g_pPlutoLogger->Write(LV_WARNING, "Error requesting Monitor mode. Waiting for next retry.");
				return;
			}
			
			g_pPlutoLogger->Write(LV_STATUS, "Expecting Acknowledge.");
			AckMessage ackmsg;
			if(ackmsg.Recv(pbusconn_)) {
				g_pPlutoLogger->Write(LV_WARNING, "Error receiving Acknowledge message.");
				return;
			}
			g_pPlutoLogger->Write(LV_STATUS, "Acknowledge received.");

			setState(STATE_READY);
		} break;
			
		case STATE_READY: {
			if(esq_.Wait(SLEEP_WAIT_MESSAGES)) {
				msq_.Lock();
				if(sendqueue_.size() > 0) {
					TelegramMessage& tlmsg = *sendqueue_.begin();
					
					g_pPlutoLogger->Write(LV_STATUS, "Sending telegram.");
					if(tlmsg.Send(pbusconn_)) {
						g_pPlutoLogger->Write(LV_WARNING, "Error sending telegram.");
						return;
					}
					g_pPlutoLogger->Write(LV_STATUS, "Telegram sent.");
					
					sendqueue_.pop_front();
				}
				msq_.Unlock();
			}
		} break;
		
		case STATE_INITIALIZEREAD: {
			g_pPlutoLogger->Write(LV_STATUS, "Data available. Checking for telegrams...");
			while(pbusconn_->isDataAvailable()) {
				TelegramMessage tlmsg;
				int ret = tlmsg.Recv(pbusconn_);
				if(ret < 0) {
					g_pPlutoLogger->Write(LV_WARNING, "Error receiving telegram. Clearing input buffers...");
					/* pbusconn_->Clear(); */
				} else
				if(ret > 0) {
					/* g_pPlutoLogger->Write(LV_STATUS, "Not a telegram message..."); */
					/* pbusconn_->Clear(); */
				} else {
					logTelegram(&tlmsg);
					if(pi_ != NULL) {
						pi_->handleTelegram(&tlmsg);
					}
					
					/* put telegram into recv queue and unlock readers */
					msq_.Lock();
					if(recvqueue_.size() >= MAX_RECVQUEUE_SIZE) {
						/* g_pPlutoLogger->Write(LV_STATUS, "Recv queue FULL. Removing front message.");*/
						recvqueue_.pop_front();
					}
					recvqueue_.push_back(tlmsg);
					msq_.Unlock();
					erq_.Post();
					
					AckMessage ackmsg;
					if(ackmsg.Send(pbusconn_)) {
						g_pPlutoLogger->Write(LV_WARNING, "Error sending Acknowledge.");
						return;
					}
				}
			}
			
			setState(STATE_READY);
		} break;
	}
}

void 
MessagePool::logTelegram(const TelegramMessage *pt) {
	g_pPlutoLogger->Write(LV_STATUS, "Telegram: graddr: %s.", pt->getGroupAddress());
}

int 
MessagePool::sendTelegram(const TelegramMessage *ptelegram) {
	msq_.Lock();
	sendqueue_.push_back(*ptelegram);
	msq_.Unlock();
	esq_.Post();
	return 0;
}

int 
MessagePool::readTelegram(TelegramMessage *ptelegram, int waitmilisecs) {
	int ret = -1;
	
	msq_.Lock();
	if(recvqueue_.size() > 0) {
		*ptelegram = *recvqueue_.begin();
		recvqueue_.pop_front();
		msq_.Unlock();
		ret = 0;
	} else {
		msq_.Unlock();
		if(erq_.Wait(waitmilisecs)) {
			msq_.Lock();
			if(recvqueue_.size() > 0) {
				*ptelegram = *recvqueue_.begin();
				recvqueue_.pop_front();
				ret = 0;
			}
			msq_.Unlock();
		}
	}
	return ret;
}

}
;
