//<-dceag-d-b->
#ifndef VDRPlugin_h
#define VDRPlugin_h

//	DCE Implemenation for #1704 VDR-Plugin

#include "Gen_Devices/VDRPluginBase.h"
//<-dceag-d-e->

#include "../Media_Plugin/Media_Plugin.h"
#include "../Media_Plugin/MediaStream.h"
#include "../Media_Plugin/MediaHandlerBase.h"
#include "EPG.h"
class VDRStateInfo;
class Row_Bookmark;

//<-dceag-decl-b->!
namespace DCE
{
	class VDRPlugin : public VDRPlugin_Command, public MediaHandlerBase, public DataGridGeneratorPlugIn
	{
//<-dceag-decl-e->
		// Private member variables
		bool m_bEPGThreadRunning;
		class Orbiter_Plugin *m_pOrbiter_Plugin;
		class Datagrid_Plugin *m_pDatagrid_Plugin;
		map<int,VDREPG::EPG *> m_mapEPG;  // Map of all grids according to the VDR Device
		VDREPG::EPG *m_mapEPG_Find(int iPK_Device) { map<int,VDREPG::EPG *>::iterator it = m_mapEPG.find(iPK_Device); return it==m_mapEPG.end() ? NULL : (*it).second; }
		map<int,VDRStateInfo *> m_mapVDRStateInfo;  // Map of all grids according to the VDR Device
		VDRStateInfo *m_mapVDRStateInfo_Find(int iPK_Device) { map<int,VDRStateInfo *>::iterator it = m_mapVDRStateInfo.find(iPK_Device); return it==m_mapVDRStateInfo.end() ? NULL : (*it).second; }
	    pluto_pthread_mutex_t m_VDRMutex; // Protect the maps
		pthread_cond_t m_VDRCond; /** < condition for the messages in the queue */

		friend class EpgGrid; // needs to use our mutex
		friend class VDRMediaStream;

		// Private methods
public:
		// Public member variables

//<-dceag-const-b->
public:
		// Constructors/Destructor
		VDRPlugin(int DeviceID, string ServerAddress,bool bConnectEventHandler=true,bool bLocalMode=false,class Router *pRouter=NULL);
		virtual ~VDRPlugin();
		virtual bool GetConfig();
		virtual bool Register();
		virtual void ReceivedCommandForChild(DeviceData_Base *pDeviceData_Base,string &sCMD_Result,Message *pMessage);
		virtual void ReceivedUnknownCommand(string &sCMD_Result,Message *pMessage);
//<-dceag-const-e->

		// Datagrids
		class DataGridTable *CurrentShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *AllShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *FavoriteChannels(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *FavoriteShows(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);
		class DataGridTable *OtherShowtimes(string GridID, string Parms, void *ExtraData, int *iPK_Variable, string *sValue_To_Assign, Message *pMessage);

		// Utilities
		class MediaDevice *GetVDRFromOrbiter(int PK_Device);
		void FetchEPG();
		VDREPG::Event *GetStartingEvent(VDREPG::EPG *pEPG,int PK_Users);
		VDREPG::Event *GetEventForBookmark(VDREPG::EPG *pEPG,Row_Bookmark *pRow_Bookmark,int &iPriority_Bookmark);

//<-dceag-const2-b->!
		/**
		* @brief
		*/
		virtual class MediaStream *CreateMediaStream( class MediaHandlerInfo *pMediaHandlerInfo, int iPK_MediaProvider, vector<class EntertainArea *> &vectEntertainArea, MediaDevice *pMediaDevice, int iPK_Users, deque<MediaFile *> *dequeFilenames, int StreamID );

		/**
		* @brief Start media playback
		*/
		virtual bool StartMedia( class MediaStream *pMediaStream );

		/**
		* @brief Stop media playback
		*/
		virtual bool StopMedia( class MediaStream *pMediaStream );

		virtual MediaDevice *FindMediaDeviceForEntertainArea(EntertainArea *pEntertainArea);


//<-dceag-h-b->
	/*
				AUTO-GENERATED SECTION
				Do not change the declarations
	*/

	/*
			*****DATA***** accessors inherited from base class
	int DATA_Get_Priority();

			*****EVENT***** accessors inherited from base class

			*****COMMANDS***** we need to implement
	*/


	/** @brief COMMAND: #65 - Jump Position In Playlist */
	/** Change channels.  +1 and -1 mean up and down 1 channel. */
		/** @param #5 Value To Assign */
			/** The track to go to.  A number is considered an absolute.  "+2" means forward 2, "-1" means back 1. */

	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign) { string sCMD_Result; CMD_Jump_Position_In_Playlist(sValue_To_Assign.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Jump_Position_In_Playlist(string sValue_To_Assign,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #185 - Schedule Recording */
	/** This will schedule a recording. */
		/** @param #68 ProgramID */
			/** The program which will need to be recorded. (The format is defined by the device which created the original datagrid) */

	virtual void CMD_Schedule_Recording(string sProgramID) { string sCMD_Result; CMD_Schedule_Recording(sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Schedule_Recording(string sProgramID,string &sCMD_Result,Message *pMessage);


	/** @brief COMMAND: #698 - Get Extended Media Data */
	/** Returns extra data about the given media, such as the title, airtime, whether it's currently scheduled to record, etc. */
		/** @param #3 PK_DesignObj */
			/** If specified the sender will be sent a goto-screen with this screen.  If not the sender will be sent a refresh */
		/** @param #68 ProgramID */
			/** If specified, the program to retrive info on.  If not specified, assumed to be the currently playing media */

	virtual void CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID) { string sCMD_Result; CMD_Get_Extended_Media_Data(sPK_DesignObj.c_str(),sProgramID.c_str(),sCMD_Result,NULL);};
	virtual void CMD_Get_Extended_Media_Data(string sPK_DesignObj,string sProgramID,string &sCMD_Result,Message *pMessage);


//<-dceag-h-e->
	};

//<-dceag-end-b->
}
#endif
//<-dceag-end-e->
