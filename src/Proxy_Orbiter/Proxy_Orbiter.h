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
#define CURRENT_SCREEN_IMAGE_TEMP   "screen_temp.png"
#define CURRENT_SCREEN_IMAGE        "screen.png"
#define CURRENT_SCREEN_XML          "screen.xml"
//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class xxProxy_Orbiter : public OrbiterSDL, public SocketListener
	{
		int	  m_iListenPort;
		int   m_ImageQuality;
		int	  m_iImageCounter;  // We will increment this each time we have a new image so we can keep track of whether a connected device has the lateest
        int   m_iLastImageSent;

		map<int,int> m_mapID_ImageCounter; // Map of connection ID's to the latest image counter
        deque<string> m_dequeXMLItems;

        string m_sRequestUrl;
        string m_sPngImageUrl;
        string m_sBaseUrl;

        pluto_pthread_mutex_t m_ActionMutex; 
        pthread_cond_t m_ActionCond;

        string sNoMediaSoftKeysXml;
        string sAllSoftKeysXml;

	public:
		xxProxy_Orbiter(int DeviceID, 
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

        virtual void Run();
        virtual void SaveXML(string sFileName);
        virtual void GenerateXMLItems(DesignObj_Orbiter *pObj); //recursive
        virtual string GenerateSoftKeys(DesignObj_Orbiter *pObj);

        virtual void ParseHardKeys();
	};
}
//-----------------------------------------------------------------------------------------------------
#endif //__xxProxy_Orbiter_H__ 
