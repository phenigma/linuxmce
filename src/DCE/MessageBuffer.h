/**
 *
 * @file MessageBuffer.h
 * @brief header file for the Command_Impl class
 * @author
 * @todo notcommented
 *
 */


#ifndef MessageBuffer_h
#define MessageBuffer_h

#include "Message.h"

namespace DCE
{
	class MessageBuffer
	{
		pthread_cond_t m_MessageBufferCond; /** < condition for the messages in the queue */
		pluto_pthread_mutex_t m_MessageBufferMutex; /** < for protecin the access to the MessageBuffer */
		class Command_Impl *m_pCommand_Impl;
		list<Message *> m_listMessage;
		Message *m_pMessage_In_Process;

	public:

		bool m_bThreadRunning;

		MessageBuffer(class Command_Impl *pCommand_Impl);
		bool BufferMessage(Message *pMessage);
		void Run();
		bool MessagesConflict(Message *pMessage1,Message *pMessage2);
	};
}

#endif
