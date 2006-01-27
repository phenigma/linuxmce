#ifndef __Proxy_Orbiter_H__
#define __Proxy_Orbiter_H__

//-----------------------------------------------------------------------------------------------------
#include <iostream> 
using namespace std; 

#include <SDL.h>
//-----------------------------------------------------------------------------------------------------
#include "../Orbiter/SDL/OrbiterSDL.h"
#include "SocketListener.h"
//-----------------------------------------------------------------------------------------------------
#define CURRENT_SCREEN_IMAGE        "screen.png"
#define CURRENT_SCREEN_XML          "screen.xml"
//-----------------------------------------------------------------------------------------------------
namespace DCE
{
	class Proxy_Orbiter : public OrbiterSDL, public SocketListener
	{

		bool IsProcessingRequest();
		void StartProcessingRequest();
		void EndProcessingRequest();
		void *PushRefreshEventWatchdog(void *);
		
		int	  m_iListenPort;
		int   m_ImageQuality;
		int	  m_iImageCounter;  // We will increment this each time we have a new image so we can keep track of whether a connected device has the lateest
        int   m_iLastImageSent;

		map<int,int> m_mapID_ImageCounter; // Map of connection ID's to the latest image counter
        deque<string> m_dequeXMLItems;
        deque<string> m_dequeCellXMLItems;

        string m_sRequestUrl;
        string m_sPngImageUrl;
        string m_sBaseUrl;

        pluto_pthread_mutex_t m_ActionMutex; 
        pluto_pthread_mutex_t m_ResourcesMutex;
		pthread_cond_t m_ActionCond;

        string sNoMediaSoftKeysXml;
        string sAllSoftKeysXml;

		bool m_bPhoneRespondedToPush;
		bool m_bProcessingRequest;
		string m_sInternalServerAddress;
		string m_sRemotePhoneIP;

	public:
		Proxy_Orbiter(int DeviceID, 
			int PK_DeviceTemplate, string ServerAddress);
		virtual ~Proxy_Orbiter();

		void StopProcessingRequest();

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

        virtual bool RenderCell( class DesignObj_DataGrid *pObj, class DataGridTable *pT, class DataGridCell *pCell, int j, int i, int iGraphicToDisplay, PlutoPoint point = PlutoPoint(0, 0) );
        virtual void RenderScreen();

        virtual void ParseHardKeys();

		virtual void RedrawObjects();
		virtual void RealRedraw( void *data );

		inline string GetDevicePngFileName();
		inline string GetDeviceXmlFileName();

		virtual void CMD_Quit(string &sCMD_Result,Message *pMessage);
		virtual void CMD_Terminate_Orbiter(string &sCMD_Result,Message *pMessage);
		virtual void OnReload();
		virtual void CMD_Regen_Screen(string &sCMD_Result,Message *pMessage);
		
		bool PushRefreshEvent(bool bForce);
		void *PushRefreshEventTask(void *p);
	};
}
//-----------------------------------------------------------------------------------------------------
#endif //__Proxy_Orbiter_H__ 
