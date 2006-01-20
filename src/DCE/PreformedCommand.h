#ifndef PreformedCommand_h
#define PreformedCommand_h
#include "Message.h"

namespace DCE
{

	class PreformedCommandResponse {
	public:
		virtual void ParseResponse(Message *pMessage)=0; 
	};

	class PreformedCommand	{
	public:
		PreformedCommandResponse *m_pcResponse; Message *m_pMessage;
		PreformedCommand() : m_pcResponse(NULL), m_pMessage(NULL) {}
		~PreformedCommand() 
		{ 
			if( m_pcResponse ) 
				delete m_pcResponse; 
		}

		void ParseResponse(Message *pMessage) 
		{ 
			if( m_pcResponse ) 
				m_pcResponse->ParseResponse(pMessage); 
		}
	};
}

#endif

