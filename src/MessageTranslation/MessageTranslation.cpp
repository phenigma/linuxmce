/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#include "MessageTranslation.h"

#include <assert.h>
#include "DCE/Logger.h"

#define POOL_IDLE_SLEEP					50
#define MARKPARAM_MESSAGE_PROCESS		((unsigned long)-1)

#ifdef WIN32
#define usleep(x) Sleep((x) / 1000)
#endif

namespace DCE {

MessageReplicatorList::MessageReplicatorList() {
	pthread_mutex_init(&mutexid_, 0);
}

MessageReplicatorList::~MessageReplicatorList() {
	pthread_mutex_destroy(&mutexid_);
}

void 
MessageReplicatorList::lock() {
	pthread_mutex_lock(&mutexid_);
}

void 
MessageReplicatorList::unlock() {
	pthread_mutex_unlock(&mutexid_);
}

/*****************************************************************
TranslationBase
*****************************************************************/
Command_Impl* 
TranslationBase::FindTargetDevice(long devid) {
	assert(getCommandImpl());
	if(!getCommandImpl()) {
		return NULL;
	}
	return FindTargetDevice(devid, getCommandImpl());
}

Command_Impl* 
TranslationBase::FindTargetDevice(long devid, Command_Impl* pfromdev) {
	if(devid == pfromdev->m_dwPK_Device) {
		return pfromdev;
	}
	
	MapCommand_Impl::iterator it = pfromdev->m_mapCommandImpl_Children.begin();
	while(it != pfromdev->m_mapCommandImpl_Children.end()) {
		Command_Impl* pcmdimpl = FindTargetDevice(devid, (*it).second);
		if(pcmdimpl) {
			return pcmdimpl;
		}
		it++;
	}
	
	return NULL;
}


		
/*****************************************************************
MessageTranslationManager
*****************************************************************/

MessageTranslationManager::MessageTranslationManager()
	: threadid_(0), stopqueue_(false)
{}

MessageTranslationManager::MessageTranslationManager(MessageTranslator* ptranslator, MessageDispatcher* pdispatcher)
{
	setTranslator(ptranslator);
	setDispatcher(pdispatcher);
}

MessageTranslationManager::~MessageTranslationManager()
{
	stopqueue_ = true;
	if(threadid_ != 0) {
		pthread_join(threadid_, 0);
	}
}

bool 
MessageTranslationManager::ProcessMessage(Message* pmsg) {
	assert(ptranslator_);
	if(ptranslator_ == NULL) {
		return false;
	};

	if(threadid_ == 0) {
		g_pPlutoLogger->Write(LV_CRITICAL,"MessageTranslationManager::Start was never called");
		return false;
	}

	bool ret = false;
	MessageReplicator msgrepl(*pmsg);

	msgqueue_.lock();
	ret = ProcessReplicator(msgrepl, msgqueue_);
	msgqueue_.unlock();
	return ret;
}

void 
MessageTranslationManager::Start() {
	if(threadid_ == 0) {
		pthread_create(&threadid_, NULL, _queueproc, (void*)this);
	}
}

bool
MessageTranslationManager::ProcessReplicator(MessageReplicator& msgrepl, MessageReplicatorList& replicators) {
	MessageReplicatorList lrepls;
	if(!ptranslator_->Translate(msgrepl, lrepls)) {
		return false;
	} else {
		MessageReplicatorList::iterator it = lrepls.begin();
		while(it != lrepls.end()) {
			if(msgrepl.getMessage().m_dwID == (*it).getMessage().m_dwID) {
				replicators.push_back(*it);
			} else {
				MessageReplicatorList prepls;
				ProcessReplicator(*it, prepls);
				for(int i = 0; i < msgrepl.getCount(); i++) {
					replicators.push_back(*it);
				}
			}
			it++;
		}
		return (replicators.size() > 0);
	}
}
/*
bool 
MessageTranslationManager::IsMessageProcessed(MessageReplicator& repl) {
	return repl.getMessage().m_mapParameters.count(MARKPARAM_MESSAGE_PROCESS) > 0;
}

void 
MessageTranslationManager::MarkMessageProcessed(MessageReplicator& repl) {
	repl.getMessage().m_mapParameters[MARKPARAM_MESSAGE_PROCESS] = "TRUE";
}
*/

/*
void 
MessageTranslationManager::_QueueProc() {
	int nextdelay = 0;
	
	handleStart();
	while(!stopqueue_) {
		if(nextdelay > 0) {
			usleep(nextdelay * 1000);
			nextdelay = 0;
		}

		bool msgpopped = false;
		
		msgqueue_.lock();
		int delay = 0;
		MessageReplicatorList::iterator it = msgqueue_.begin();
		if(it != msgqueue_.end()) {
			MessageReplicator& replmsg = (*it);
			delay = replmsg.getPreDelay();
			nextdelay = replmsg.getPostDelay();
			
			replmsg.setCount(replmsg.getCount() - 1);
			
			assert(pdispatcher_);
			if(pdispatcher_) {
				MarkMessageProcessed(replmsg);
				pdispatcher_->DispatchMessage(replmsg);
			}
			
			if(replmsg.getCount() <= 0) {
				msgqueue_.erase(it);
			}
			msgpopped = true;
		}
		msgqueue_.unlock();
		
		if(msgpopped && delay > 0) {
			usleep(delay * 1000);
		} else {
			usleep(POOL_IDLE_SLEEP * 1000);
		}
	}
	handleStop();
}

*/

void 
MessageTranslationManager::_QueueProc() {
	handleStart();
	while(!stopqueue_) {
		msgqueue_.lock();
		MessageReplicatorList::iterator it = msgqueue_.begin();
		if(it != msgqueue_.end()) {
			MessageReplicator replmsg = (*it);
			msgqueue_.erase(it);
			msgqueue_.unlock();
			
			assert(pdispatcher_);
			if(pdispatcher_) {
				pdispatcher_->DispatchMessage(replmsg);
			}
		} else {
			msgqueue_.unlock();
			usleep(POOL_IDLE_SLEEP * 1000);
		}
	}
	handleStop();
}
		
void* 
MessageTranslationManager::_queueproc(void *arg) {
	MessageTranslationManager* pme = (MessageTranslationManager*)arg;
	pme->_QueueProc();
	return 0;
}

};
