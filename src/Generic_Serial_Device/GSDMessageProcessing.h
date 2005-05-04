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
#ifndef DCEGSDMESSAGEPROCESSING_H
#define DCEGSDMESSAGEPROCESSING_H

#include "AVMessageTranslation.h"

namespace DCE {

class GSDMessageTranslator : public AVMessageTranslator {
public:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls);
};

class GSDMessageDispatcher : public AVMessageDispatcher {
public:
	/*to be continued...*/
};

typedef DefaultMessageProcessor<GSDMessageTranslator, GSDMessageDispatcher> GSDMessageProcessor;

};

#endif
