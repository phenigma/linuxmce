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

#include "pluto_main/Database_pluto_main.h"
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
	void setDatabase(Database_pluto_main* pdb);
	Database_pluto_main* getDatabase();

private:
	bool dbprovided_;
	Database_pluto_main* pdb_;
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
		: pcmdimpl_(NULL) {}	
public:
	Command_Impl* getCommandImpl() {
		return pcmdimpl_;
	}
	void setCommandImpl(Command_Impl* pcmdimpl) {
		pcmdimpl_ = pcmdimpl;
	}

private:
	Command_Impl* pcmdimpl_;
};

};

#endif
