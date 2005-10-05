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
#ifndef DCEDEFAULTMESSAGETRANSLATION_H
#define DCEDEFAULTMESSAGETRANSLATION_H

#include "MessageTranslation.h"

namespace DCE {


/*****************************************************************
DefaultMessageTranslator
*****************************************************************/

class DefaultMessageTranslator : public MessageTranslator {
protected:
	DefaultMessageTranslator();
	virtual ~DefaultMessageTranslator();

protected:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls);

protected:

private:
	std::map<int,int> map_PowerDelay;
};

/*****************************************************************
DefaultMessageDispatcher
*****************************************************************/

class DefaultMessageDispatcher : public MessageDispatcher {
protected:
	virtual void DispatchMessage(Message* pmsg) = 0;
	
protected:
	virtual void DispatchMessage(MessageReplicator& inrepl);
};

/*****************************************************************
DefaultMessageProcessor
*****************************************************************/

template <class T = DefaultMessageTranslator, class D = DefaultMessageDispatcher>
class DefaultMessageProcessor : public MessageProcessor<T, D>  {
public:
	DefaultMessageProcessor() 
		: pAllDevices_(NULL) {}	
public:
	AllDevices *getAllDevices() {
		return pAllDevices_;
	}
	void setAllDevices(AllDevices *pAllDevices) {
		pAllDevices_ = pAllDevices;
	}

private:
	AllDevices *pAllDevices_;
};

};

#endif
