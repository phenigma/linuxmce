#ifndef __xxProxy_Orbiter_H__
#define __xxProxy_Orbiter_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream> 
using namespace std; 

#include <SDL.h>
//-----------------------------------------------------------------------------------------------------
#include "../Orbiter/SDL/OrbiterSDL.h"
#include "SocketListener.h"
//-----------------------------------------------------------------------------------------------------
#define CURRENT_SCREEN  "TmpScreen.png"

//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class xxProxy_Orbiter : public OrbiterSDL, public SocketListener
	{
		int	  m_iListenPort;
		int   m_ImageQuality;
		int	  m_iImageCounter;  // We will increment this each time we have a new image so we can keep track of whether a connected device has the lateest
		map<int,int> m_mapID_ImageCounter; // Map of connection ID's to the latest image counter

	public:
		xxProxy_Orbiter(int ListenPort, int DeviceID, 
			int PK_DeviceTemplate, string ServerAddress);
		virtual ~xxProxy_Orbiter();

		// Public virtual methods
		virtual void DisplayImageOnScreen(struct SDL_Surface *pScreenImage);

		virtual void BeginPaint();
		virtual void EndPaint();

		virtual bool GetConfig();

		virtual void SetImageQuality(unsigned long nImageQuality);

		virtual bool ReceivedString( Socket *pSocket, string sLine, int nTimeout = - 1 );
		virtual void ReceivedMessage( Socket *pSocket, Message* pMessage ) {} // We don't do messages
	};
}
//-----------------------------------------------------------------------------------------------------
#endif //__xxProxy_Orbiter_H__ 
