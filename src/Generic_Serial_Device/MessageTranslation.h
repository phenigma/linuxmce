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
#ifndef DCEMESSAGETRANSLATION_H
#define DCEMESSAGETRANSLATION_H

#include "DCE/Message.h"

namespace DCE {

/**
@author Igor Spac,,,
*/

/*****************************************************************
MessageTimedQueue
*****************************************************************/

class MessageTimedQueue {
public:
	MessageTimedQueue();
	~MessageTimedQueue();

public:
	void PushToBack(const Message *pmsg, int count = 1, int delay = 0);
	bool PopMessage(Message *pmsg);

private:
	typedef struct _TIMEDMSG {
		_TIMEDMSG(const Message *pmsg, int count, int postdelay) 
			: msg_(*pmsg), count_(count), postdelay_(postdelay), predelay_(0)
		{}
		
		Message msg_;
		int count_;
		int postdelay_;
		int predelay_;
	} TIMEDMSG;
	typedef std::list<TIMEDMSG> TIMEDMSGLIST;
	
	TIMEDMSGLIST msgqueue_;
	int predelay_;

private:
	pthread_mutex_t mutexid_;
};

/*****************************************************************
MessageTranslator
*****************************************************************/

class MessageTranslator {
public:
	virtual bool Translate(Message* pmsg, MessageTimedQueue* pqueue) = 0;
};

/*****************************************************************
DefaultMessageTranslator
*****************************************************************/

class DefaultMessageTranslator : public MessageTranslator {
public:
	virtual bool Translate(Message* pmsg, MessageTimedQueue* pqueue);
};


/*****************************************************************
MessageTranslationManager
*****************************************************************/
class MessageTranslationManager {
public:
	MessageTranslationManager();
	MessageTranslationManager(MessageTranslator* ptranslator);
	virtual ~MessageTranslationManager();
	
public:
	bool ProcessMessage(Message* pmsg, MessageTranslator* ptranslator = NULL);

public:
	void setDefaultTranslator(MessageTranslator* ptranslator) {
		deftranslator_ = ptranslator;
	}
	
	MessageTranslator* getDefaultTranslator() {
		return deftranslator_;
	}

protected:
	virtual void DispatchMessage(Message* pmsg) = 0;

private:
	pthread_t threadid_;
	bool stopqueue_;
	void _QueueProc();
	static void* _queueproc(void *arg);

private:
	MessageTranslator* deftranslator_;
	MessageTimedQueue msgqueue_;
};

/*****************************************************************
MessageProcessor
*****************************************************************/

class MessageProcessor : 
		protected MessageTranslationManager, 
		protected MessageTranslator 
{
public:
	MessageProcessor();
	
public:
	bool ProcessMessage(Message* pmsg);
};

};

#endif
