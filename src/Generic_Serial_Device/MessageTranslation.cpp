//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "MessageTranslation.h"

#define POOL_IDLE_SLEEP		50

namespace DCE {

MessageTimedQueue::MessageTimedQueue() 
	: predelay_(0)
{
	pthread_mutex_init(&mutexid_, 0);
}

MessageTimedQueue::~MessageTimedQueue() {
	pthread_mutex_destroy(&mutexid_);
}

void 
MessageTimedQueue::PushToBack(const Message *pmsg, int count, int postdelay) {
	TIMEDMSG msg(pmsg, count, postdelay);
	pthread_mutex_lock(&mutexid_);
	msgqueue_.push_back(msg);
	pthread_mutex_unlock(&mutexid_);
}

bool 
MessageTimedQueue::PopMessage(Message *pmsg) {
	bool msgpopped = false;
	
	if(predelay_ > 0) {
		usleep(predelay_ * 1000);
	}
	
	int predelay = 0;
	pthread_mutex_lock(&mutexid_);
	TIMEDMSGLIST::iterator it = msgqueue_.begin();
	if(it != msgqueue_.end()) {
		TIMEDMSG& msg = (*it);
		predelay = msg.predelay_;
		predelay_ = msg.postdelay_;
		
		msg.count_--;
		*pmsg = msg.msg_;
		
		if(msg.count_ <= 0) {
			msgqueue_.erase(it);
		}
		
		msgpopped = true;
	}
	pthread_mutex_unlock(&mutexid_);
	if(msgpopped && predelay > 0) {
		usleep(predelay * 1000);
	}
	return msgpopped;
}

MessageProcessor::MessageProcessor() {
	setDefaultTranslator(this);
}

bool 
MessageProcessor::ProcessMessage(Message* pmsg) {
	return MessageTranslationManager::ProcessMessage(pmsg, this);
}


MessageTranslationManager::MessageTranslationManager()
	: threadid_(0), stopqueue_(false)
{}

MessageTranslationManager::MessageTranslationManager(MessageTranslator* ptranslator)
{
	setDefaultTranslator(ptranslator);
}

MessageTranslationManager::~MessageTranslationManager()
{
	stopqueue_ = true;
	if(threadid_ != 0) {
		pthread_join(threadid_, 0);
	}
}

bool 
MessageTranslationManager::ProcessMessage(Message* pmsg, MessageTranslator* ptranslator)
{
	if(ptranslator == NULL) {
		ptranslator = getDefaultTranslator();
	};

	if(threadid_ == 0) {
		pthread_create(&threadid_, NULL, _queueproc, (void*)this);
	}
		
	if(ptranslator != NULL) {
		return ptranslator->Translate(pmsg, &msgqueue_);
	} else {
		return false;
	}
}

void 
MessageTranslationManager::_QueueProc() {
	while(!stopqueue_) {
		Message msg;
		if(msgqueue_.PopMessage(&msg)) {
			DispatchMessage(&msg);
		} else {
			usleep(POOL_IDLE_SLEEP * 1000);
		}
	}
}
		
void* 
MessageTranslationManager::_queueproc(void *arg) {
	MessageTranslationManager* pme = (MessageTranslationManager*)arg;
	pme->_QueueProc();
	return 0;
}

};
