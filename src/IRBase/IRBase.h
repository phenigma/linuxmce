#ifndef IRBASE
#define IRBASE

#include "DCE/Command_Impl.h"
#include "MessageTranslation/AVMessageTranslation.h"
#include "PlutoUtils/MultiThreadIncludes.h"
#include "PlutoUtils/Other.h"

using namespace DCE;

class IRBase : public AVMessageProcessor {
public:
	IRBase() {}
	virtual ~IRBase() {};

public:
	typedef pair<long, long> longPair;
	map<longPair, std::string>& getCodeMap() {
		return codemap_;
	}
	
		
protected:
	virtual bool Translate(MessageReplicator& inrepl, MessageReplicatorList& outrepls);
	virtual void DispatchMessage(Message* pmsg);
	
protected:
	/*exposed methods*/
	virtual void SendIR(string Port, string IRCode) = 0;

	virtual void handleStart();
	virtual void handleStop();

private:
	void PushSendCodeMessage(Message* porigmsg, MessageReplicatorList& outrepls, 
				long cmdid, const std::string& port, int count = 1, int delay = 0);
	
private:
	map<longPair, std::string> codemap_;		// maps device,command pair to IR code
};

#endif
