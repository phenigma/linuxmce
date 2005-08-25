/*
	Copyright (C) 2004 Pluto, Inc., a Florida Corporation

	http://www.plutohome.com

	Phone: +1 (877) 758-8648

	This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
	of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	See the GNU General Public License for more details.
*/

#ifndef DCEMESSAGETRANSLATION_H
#define DCEMESSAGETRANSLATION_H

#include "DCE/Message.h"
#include "DCE/Command_Impl.h"

namespace DCE {

/**
@author Igor Spac,,,
*/

/*****************************************************************
MessageReplicator
*****************************************************************/
class MessageReplicator {
public:
	MessageReplicator(const Message& msg, int count = 1, int predelay = 0, int postdelay = 0) 
		: msg_(msg), count_(count), predelay_(predelay), postdelay_(postdelay)
	{}

public:
	Message& getMessage() {
		return msg_;
	}
	
	int getCount() {
		return count_;
	}
	void setCount(int count) {
		count_ = count;
	}
	
	int getPreDelay() {
		return predelay_;
	}
	void setPreDelay(int predelay) {
		predelay_ = predelay;
	}
	
	int getPostDelay() {
		return postdelay_;
	}
	void setPostDelay(int postdelay) {
		postdelay_ = postdelay;
	}

private:
	Message msg_;
	int count_;
	int predelay_;
	int postdelay_;
};

/*****************************************************************
MessageReplicatorList
*****************************************************************/
class MessageReplicatorList : public std::list<MessageReplicator> {
public:
	MessageReplicatorList();
	virtual ~MessageReplicatorList();

public:
	void lock();
	void unlock();
		
private:
	pthread_mutex_t mutexid_;	
};

/*****************************************************************
TranslationBase
*****************************************************************/
class TranslationBase {
public:
	Command_Impl* FindTargetDevice(long devid);

protected:
	virtual Command_Impl* getCommandImpl() = 0;
	
private:
	static Command_Impl* FindTargetDevice(long devid, Command_Impl* pfromdev);
};

/*****************************************************************
MessageTranslator
*****************************************************************/

class MessageTranslator : public TranslationBase {
public:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) = 0;
};

/*****************************************************************
MessageDispatcher
*****************************************************************/

class MessageDispatcher : public TranslationBase {
public:
	virtual void DispatchMessage(MessageReplicator& inrepl) = 0;
};

/*****************************************************************
MessageTranslationManager
*****************************************************************/
class MessageTranslationManager {
public:
	MessageTranslationManager();
	MessageTranslationManager(MessageTranslator* ptranslator, MessageDispatcher* pdispatcher);
	virtual ~MessageTranslationManager();
	
public:
	bool ProcessMessage(Message* pmsg);

public:
	void setTranslator(MessageTranslator* ptranslator) {
		ptranslator_ = ptranslator;
	}
	
	MessageTranslator* getTranslator() {
		return ptranslator_;
	}
	
	void setDispatcher(MessageDispatcher* pdispatcher) {
		pdispatcher_ = pdispatcher;
	}
	
	MessageDispatcher* setDispatcher() {
		return pdispatcher_;
	}

protected:
	virtual void handleStart() {};
	virtual void handleStop() {};

	virtual void Start();

private:
/*	bool IsMessageProcessed(MessageReplicator& repl);
	void MarkMessageProcessed(MessageReplicator& repl);
*/
	bool ProcessReplicator(MessageReplicator& repl, MessageReplicatorList& replicators);
	
private:
	pthread_t threadid_;
	bool stopqueue_;
	void _QueueProc();
	static void* _queueproc(void *arg);

private:
	MessageTranslator* ptranslator_;
	MessageDispatcher* pdispatcher_;
	MessageReplicatorList msgqueue_;
};

/*****************************************************************
MessageProcessor
*****************************************************************/

template <class T = MessageTranslator, class D = MessageDispatcher>
class MessageProcessor : 
		public MessageTranslationManager,
		protected T, 
		protected D 
{
public:
	MessageProcessor() {
		setTranslator(this);
		setDispatcher(this);
	}
	
public:
	bool ProcessMessage(Message* pmsg) {
		return MessageTranslationManager::ProcessMessage(pmsg);
	}
};

};

#endif
