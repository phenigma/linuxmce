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
		: pdevicedata_(NULL) {}	
public:
	DeviceData_Base* getDeviceData() {
		return pdevicedata_;
	}
	void setDeviceData(DeviceData_Base* pdevicedata) {
		pdevicedata_ = pdevicedata;
	}

private:
	DeviceData_Base * pdevicedata_;
};

};

#endif
