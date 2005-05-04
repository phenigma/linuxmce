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
#include "GSDMessageProcessing.h"

namespace DCE {

bool GSDMessageTranslator::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	if(AVMessageTranslator::Translate(inrepl, outrepls)) {
		return true;
	} else {
		/*in all cases we must route messages to GSD devices*/
		outrepls.push_back(inrepl);
		return true;
	}
};

};
