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
#ifndef DCEAVMESSAGETRANSLATION_H
#define DCEAVMESSAGETRANSLATION_H

#include "DefaultMessageTranslation.h"

namespace DCE {

class AVMessageTranslator : public DefaultMessageTranslator {
public:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls);

public:
	map<int, bool> lastcmdwaspause_; // So if we get 2 pauses in a row, we convert the 2nd one to a play
	map<int, bool> laststatus_power_;
	map<int, int> laststatus_input_;
};

class AVMessageDispatcher : public DefaultMessageDispatcher {
public:
	/*to be continued...*/
};

typedef DefaultMessageProcessor<AVMessageTranslator, AVMessageDispatcher> AVMessageProcessor;

};

#endif
