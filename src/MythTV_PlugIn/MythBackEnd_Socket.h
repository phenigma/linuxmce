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
	class MythTV_PlugIn;

	class MythBackEnd_Socket_Wrapper : public ClientSocket
	{
		class MythBackEnd_Socket *m_pMythBackEnd_Socket;

	public:
		MythBackEnd_Socket_Wrapper(MythBackEnd_Socket *pMythBackEnd_Socket,string sIP)
			: ClientSocket(0,sIP,"MythPluginSocket")
		{ 
			m_pMythBackEnd_Socket=pMythBackEnd_Socket; 
		}
		virtual ~MythBackEnd_Socket_Wrapper() {}

		// overriding on connect event handler, because we use custom protocol
		virtual bool OnConnect( int PK_DeviceTemplate,string sExtraInfo="" ){return true;};

		virtual void Close();
	};

	class MythBackEnd_Socket
	{
	private:
		bool m_bConnected;
		bool InternalSendMythString(string sValue,string *sResponse); // For internal use only.  Use SendMythString which will retry on 1 failure
		MythBackEnd_Socket_Wrapper *m_pSocket;
		string m_sIPAddress;
		MythTV_PlugIn *m_pMythTV_PlugIn;
void PurgeSocketBuffer();  // move to socket.h

	public:
		
		MythBackEnd_Socket(MythTV_PlugIn *pMythTV_PlugIn,string sIPAddress);

		virtual bool Connect( );

		void Close();

		void DeleteSocket()
		{
			delete m_pSocket;
			m_pSocket=NULL;
			m_bConnected=false;
		}

		bool SendMythString(string sValue,string *sResponse=NULL);
		bool SendMythStringGetOk(string sValue);
	};
}

#endif
