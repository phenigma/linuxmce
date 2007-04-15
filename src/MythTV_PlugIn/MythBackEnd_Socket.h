/**
 *
 * @file MythBackEnd_Socket.h
 * @brief header file for the  MythTvStream, MythBackEnd_Socket classes
 *
 */

#ifndef MythBackEnd_Socket_h
#define MythBackEnd_Socket_h

#include "DCE/ClientSocket.h"

namespace DCE
{
	class MythBackEnd_Socket : public ClientSocket
	{
	private:
		bool m_bConnected;
void PurgeSocketBuffer();  // move to socket.h

	public:
		
		MythBackEnd_Socket(string sIPAddress);

		// overriding on connect event handler, because we use custom protocol
		virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" ){return true;};

		virtual bool Connect( int PK_DeviceTemplate=0,string sExtraInfo="" );
		virtual void Close();

		bool SendMythString(string sValue,string *sResponse=NULL);
		bool SendMythStringGetOk(string sValue);
	};
}

#endif
