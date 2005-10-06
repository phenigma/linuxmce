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
#include "RubyIOManager.h"
#include "DCE/Logger.h"
namespace DCE {

bool GSDMessageTranslator::Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls) {
	if(AVMessageTranslator::Translate(inrepl, outrepls)) {
		MessageReplicator *new_msg=&outrepls.back();
		Message* pmsg = &new_msg->getMessage();
		RubyIOManager* pmanager = RubyIOManager::getInstance();	
		g_pPlutoLogger->Write(LV_STATUS, "GSDMessageTranslator after translate.");
		if(	!pmanager->getCodeSupplier().isCmdImplemented(pmsg->m_dwPK_Device_To, pmsg->m_dwID))
		{
			outrepls.pop_back();		
			outrepls.push_back(inrepl);
			g_pPlutoLogger->Write(LV_STATUS, "GSDMessageTranslator will try to send untranslated.");
		}
		return true;
	} else {
		/*in all cases we must route messages to GSD devices*/
		outrepls.push_back(inrepl);
		return true;
	}
};

};
